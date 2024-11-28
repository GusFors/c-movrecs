#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "sorting.h"
#include "readfiles.h"
#include "calcs.h"
#include <pthread.h>
#include "regression_tests.h"

#define NUM_THREADS 4

unsigned int count_user_ratings(unsigned int userid_a, struct rating *filtered_ratings, unsigned int filtered_rlength) {
  unsigned int numratings_a = 0;
  unsigned int numratings_other = 0;

  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a)
      numratings_a++;
    else
      numratings_other++;
  }

  return numratings_a;
}

unsigned int weighted_scores_short(struct user_sim *simscores, struct rating *ratings_notseen, unsigned int simlen, unsigned int notseen_cnt,
                                   struct weighted_score *wscores) {
  unsigned int ws_len = 0;
  struct timespec t1, t2;

  clock_gettime(CLOCK_MONOTONIC, &t1);
  merg_sort_rating_by_uid(ratings_notseen, notseen_cnt, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  printf("rating by uid time: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_nsec - t1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  unsigned int counted_indexes = 0;
  unsigned int is_curr_id = 0;

  for (unsigned int i = 0, l = 0; i < simlen; i++) {
    for (unsigned int y = counted_indexes; y < notseen_cnt; y++) {
      if (simscores[i].user_id == ratings_notseen[y].user_id) {

        if (is_curr_id == 0)
          is_curr_id = 1;

        wscores[l].movie_id = ratings_notseen[y].movie_id;
        wscores[l].simscore = simscores[i].simscore;
        wscores[l].weighted_rating = (simscores[i].simscore * ratings_notseen[y].rating);

        l++;
        ws_len++;
      } else if (is_curr_id && simscores[i].user_id != ratings_notseen[y].user_id) {
        is_curr_id = 0;
        counted_indexes = y;
        break;
      }
    }
  }

  return ws_len;
}

unsigned int euclidianscores(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids, unsigned int ulength,
                             struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores) {
  unsigned int userid_b = 0;
  unsigned int simlen = 0;
  unsigned int checked_rindex = 0;

  for (unsigned int i = 0, l = 0; i < ulength; i++) {
    unsigned int is_curr_user = 0;
    userid_b = uids[i];

    float sim = 0;
    unsigned int n = 0;

    for (unsigned int y = checked_rindex; y < filtered_rlength; y++) {
      if (filtered_ratings[y].user_id == userid_b) {
        if (is_curr_user == 0) {
          is_curr_user = 1;
        }
      } else if (is_curr_user && (filtered_ratings[y].user_id != userid_b)) {
        is_curr_user = 0;
        checked_rindex = y;
        break;
      }

      // collect matching values from a before to skip movid check? possibly also split ratings into smaller arrays to fit in cache
      for (unsigned int j = 0; j < numratings_a; j++) {
        if (filtered_ratings[y].movie_id == ratings_a[j].movie_id) {
          sim += ((ratings_a[j].rating - filtered_ratings[y].rating) * (ratings_a[j].rating - filtered_ratings[y].rating));
          n += 1;
        }
      }
    }

    if (uids[i] == userid_a)
      continue;

    if (n == 0)
      continue;

    float inv = 1.0 / (1.0 + sim);

    if (inv > 0.0) {
      simscores[simlen].user_id = userid_b; // check if 0?
      simscores[simlen].simscore = inv;
      simlen++;
    }
  }
  for (int i = 0; i < simlen; i++) {
    // printf("simscores[%d] uid: %d, simscore: %f\n", i, simscores[i].user_id, simscores[i].simscore);
    // printf("{%d, %f},\n", simscores[i].user_id, simscores[i].simscore);
  }
  printf("return simlen: %d\n", simlen);
  return simlen;
}

struct user_rating_chunk {
  unsigned int length;
  unsigned int *userids;
  unsigned int ulength;
  struct rating *ratings;
};

unsigned int euclidianscores_chunk(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids,
                                   unsigned int ulength, struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores) {

  unsigned int movids_a[numratings_a];
  printf("uids: %d\n", ulength);
  // unsigned int *ratings_uids = malloc(filtered_rlength * sizeof(unsigned int));
  // unsigned int *ratings_movids = malloc(filtered_rlength * sizeof(unsigned int));
  // float *ratings_r = malloc(filtered_rlength * sizeof(float));

  // struct rating *ratings_chunk1 = malloc((filtered_rlength / 2) * sizeof(struct rating));
  // struct rating *ratings_chunk2 = malloc((filtered_rlength / 2) * sizeof(struct rating));

  unsigned int num_chunks = 2; // check simline, check chunk bounds if not divisible? // affects simlen and possibly scores
  unsigned int chunk_left = 0;
  int chunk_split = filtered_rlength / num_chunks; // check if not even divison, signed so chunk_split_offset can be compared
  unsigned int adjusted_split = 0;
  unsigned int chunk_stop_ids[num_chunks];

  for (int i = 0, l = chunk_split; i < num_chunks; i++, l += chunk_split) {
    chunk_stop_ids[i] = filtered_ratings[l].user_id;
    if (i == num_chunks - 1) {
      chunk_stop_ids[i] = filtered_ratings[filtered_rlength - 1].user_id;
    }
    printf("l: %d\n", l);
  }

  struct user_rating_chunk rating_chunks[num_chunks];
  for (unsigned int i = 0; i < num_chunks; i++) {
    rating_chunks[i].ratings = malloc((filtered_rlength) * sizeof(struct rating));
    rating_chunks[i].userids = malloc((ulength) * sizeof(int));
  }

  int chunk_split_offset = 0;
  for (int i = 0; i < num_chunks; i++) {
    printf("chunk[%d] stop id: %d\n", i, chunk_stop_ids[i]);

    int j = chunk_split_offset;

    printf("j1: %d, first index: %d, first uid: %d, first rating movid: %d\n", j, (chunk_split * i) + j,
           filtered_ratings[(chunk_split * i) + j].user_id, filtered_ratings[(chunk_split * i) + j].movie_id);

    while (j < (chunk_split) && filtered_ratings[(chunk_split * i) + j].user_id < chunk_stop_ids[i]) { // cuts uid ratings, <= ?
      rating_chunks[i].ratings[j + (-chunk_split_offset)] = filtered_ratings[(chunk_split * i) + j];
      j++;
    }

    // if (i > 0)
    // if (i == num_chunks - 1)
    if (i == num_chunks - 1 || i > 0) {
      int y = j;
      for (; y < (chunk_split); y++) {
        rating_chunks[i].ratings[y] = filtered_ratings[(chunk_split * i) + y];
        // if (filtered_ratings[(chunk_split * i) + y].user_id == 318) {
        //   printf("2rating uid: %d, movid: %d\n", filtered_ratings[(chunk_split * i) + y].user_id, filtered_ratings[(chunk_split * i) +
        //   y].movie_id);
        // }
      }
      j = y;
    }

    // if (j < chunk_split) {

    printf("j: %d, stopping early, even split would be: %d, diff: %d, new offset: %d\n", j, chunk_split, chunk_split - j, chunk_split_offset);
    chunk_split_offset = 0;
    chunk_split_offset -= (chunk_split - j);
    // }

    rating_chunks[i].length = j;

    printf("chunk[%d] length: %d, first id: %d, last id: %d\n", i, rating_chunks[i].length, rating_chunks[i].ratings[0].user_id,
           rating_chunks[i].ratings[j - 1].user_id);

    unsigned int current_uid = 0;
    unsigned int first_uid = rating_chunks[i].ratings[0].user_id;
    printf("first id: %d\n", first_uid);
    // usleep(2000000);

    // unsigned int user_cnt = rating_chunks[i].ratings[0].user_id - 1;
    unsigned int user_cnt = rating_chunks[i].ratings[0].user_id - 1;
    // unsigned int user_cnt = 0;
    unsigned int chunk_ulength = 0;

    for (int l = 0; l < rating_chunks[i].length; l++) {
      // current_uid = rating_chunks[i].ratings[(chunk_split * i) + j].user_id;
      current_uid = rating_chunks[i].ratings[l].user_id;

      if (user_cnt != current_uid && current_uid != 0) {
        // printf("usercnt: %d, l: %d, current uid: %d\n", user_cnt, l, current_uid);
        // printf("curr id: %d, usercnt: %d\n", current_uid, user_cnt);
        // usleep(100);
        // rating_chunks[i].userids[user_cnt] = current_uid;

        rating_chunks[i].userids[chunk_ulength] = current_uid;
        user_cnt++;
        chunk_ulength++;
        // if (current_uid == 610)
        //   break;
      }
      // printf("l: %d\n", l);
    }

    // printf("userid for 318: %d\n", rating_chunks[1].userids[0]);
    // rating_chunks[i].ulength = user_cnt;
    rating_chunks[i].ulength = chunk_ulength; // +1 ?
    // if (i == num_chunks - 1)
    //   rating_chunks[i].ulength++;
    // rating_chunks[i].ulength = user_cnt - rating_chunks[i].ratings[0].user_id;
    // printf("uids: %d\n", ulength);
  }

  for (int i = 0; i < numratings_a; i++)
    movids_a[i] = ratings_a[i].movie_id;

  unsigned int simlen = 0;
  // printf("movids a: %d\n", movids_a[0]);
  for (int z = 0; z < num_chunks; z++) {
    for (int x = 0; x < rating_chunks[z].length; x++) {
      if (rating_chunks[z].ratings[x].user_id == 318) { // printf("chunk[%d] ratings[%d] movid: %d\n", z, x, rating_chunks[z].ratings[x].movie_id);
      } // doesn't take all uid 318 ratings
    }

    printf("movids a: %d\n", movids_a[0]);
    printf("rating_chunks[%d].ulength: %d\n", z, rating_chunks[z].ulength);

    // unsigned int userid_b = 0;
    // unsigned int userid_b = rating_chunks[z].ratings[0].user_id - 1;
    unsigned int userid_b = rating_chunks[z].ratings[0].user_id;
    unsigned int is_curr_user = 0;
    unsigned int checked_rindex = 0;

    for (unsigned int i = 0, l = 0; i < rating_chunks[z].ulength; i++) {
      // printf("userid[0]: chunk: %d, original: %d, ulength: %d\n", rating_chunks[z].ratings[i].user_id, uids[i], ulength);

      // userid_b = uids[i];
      userid_b = rating_chunks[z].userids[i];

      float sim = 0;
      unsigned int n = 0;

      for (unsigned int y = checked_rindex; y < rating_chunks[z].length; y++) {
        if (rating_chunks[z].ratings[y].user_id == userid_b) {
          if (is_curr_user == 0) {
            is_curr_user = 1;
          }
        } else if (is_curr_user && (rating_chunks[z].ratings[y].user_id != userid_b)) {
          is_curr_user = 0;
          checked_rindex = y;
          break;
        }

        // collect matching values from a before to skip movid check? possibly also split ratings into smaller arrays to fit in cache
        for (unsigned int j = 0; j < numratings_a; j++) {
          if (rating_chunks[z].ratings[y].movie_id == movids_a[j]) {
            sim += ((ratings_a[j].rating - rating_chunks[z].ratings[y].rating) * (ratings_a[j].rating - rating_chunks[z].ratings[y].rating));
            n += 1;
          }
        }
        //  for (unsigned int j = 0; j < numratings_a; j++) {
        //   if (ratings_movids[y] == movids_a[j]) {
        //     sim += ((ratings_a[j].rating - ratings_r[y]) * (ratings_a[j].rating - ratings_r[y]));
        //     n += 1;
        //   }
        // }
      }

      if (rating_chunks[z].userids[i] == userid_a)
        continue;

      if (n == 0) // check sim instead of n?
        continue;

      float inv = 1.0 / (1.0 + sim);

      if (inv > 0.0) {
        simscores[simlen].user_id = userid_b;
        simscores[simlen].simscore = inv;
        simlen++;
      }
    }
    // checked_rindex = 0;
  }

  for (int i = 0; i < simlen; i++) {
    if (simscores[i].user_id > 605)
      printf("simscores[%d] uid: %d, simscore: %f\n", i, simscores[i].user_id, simscores[i].simscore); // uid 318 missing?
    // simscores[394] uid: 608, simscore: 0.015504 // correct one,
    // simscores[395] uid: 610, simscore: 0.032787
  }
  printf("return simlen: %d\n", simlen);
  return simlen;
}

void get_recommendations(struct movie *movies, struct rating *ratings, unsigned int *uids, unsigned int mlength, unsigned int rlength,
                         unsigned int ulength) {
  struct timespec total1, total2, calc1, calc2, m2;
  clock_gettime(CLOCK_MONOTONIC, &total1);

  clock_gettime(CLOCK_MONOTONIC, &calc1);
  merg_sort_rating_by_movid(ratings, rlength, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &m2);

  printf("sort rating movids in: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(m2.tv_sec - calc1.tv_sec) + (double)(m2.tv_nsec - calc1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  calc_num_ratings(movies, ratings, mlength, rlength);

  unsigned int min_numratings = 3;
  struct rating *filtered_ratings = malloc(sizeof(struct rating) * rlength);
  struct movie_compact *filtered_movies = malloc(sizeof(struct movie_compact) * mlength);

  unsigned int filtered_mlength = filter_movie_numratings(movies, mlength, min_numratings, filtered_movies);
  unsigned int filtered_rlength = filter_numratings(movies, ratings, mlength, rlength, min_numratings, filtered_ratings);
  // printf("filtered rlength: %d\n", filtered_rlength);

  unsigned int userid_a = 3;
  unsigned int numratings_a = count_user_ratings(userid_a, filtered_ratings, filtered_rlength);
  unsigned int *movseen_by_userid_a = malloc(numratings_a * sizeof(unsigned int));

  for (unsigned int i = 0, y = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      movseen_by_userid_a[y] = filtered_ratings[i].movie_id;
      y++;
    }
  }
  printf("movseen num by a: %d\n", numratings_a);

  clock_t r1 = clock();
  struct rating ratings_a[numratings_a];

  printf("rlength: %d\n", filtered_rlength);

  for (unsigned int i = 0, y = 0, j = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      ratings_a[y] = filtered_ratings[i];
      y++;
    }
  }

  clock_t notseen_t1 = clock();
  unsigned int notseen_cnt = 0;
  for (unsigned int i = 0, y = 0, j = 0; i < filtered_rlength; i++) {
    unsigned int isseen = 0;
    for (unsigned int z = 0; z < numratings_a; z++) {
      if (movseen_by_userid_a[z] == filtered_ratings[i].movie_id) {
        isseen = 1;
        // j++;
      }
    }
    if (!isseen)
      notseen_cnt++;
  }

  struct rating *ratings_notseen = malloc(notseen_cnt * sizeof(struct rating));

  unsigned int notseen_cnt2 = 0;
  for (unsigned int i = 0, y = 0, j = 0; i < filtered_rlength; i++) {
    unsigned int isseen = 0;
    for (unsigned int z = 0; z < numratings_a; z++) {
      if (movseen_by_userid_a[z] == filtered_ratings[i].movie_id) {
        isseen = 1;
        // j++;
      }
    }
    if (!isseen) {
      ratings_notseen[notseen_cnt2] = filtered_ratings[i];
      notseen_cnt2++;
    }
  }

  printf("notseen ratings in %.17gms\n", ((float)(clock() - notseen_t1) / CLOCKS_PER_SEC) * 1000);

  unsigned int userlen = ulength - 1;
  struct user_sim *simscores = malloc(userlen * sizeof(struct user_sim));
  printf("ulength: %d\n", ulength);

  struct timespec u1, u2;
  clock_gettime(CLOCK_MONOTONIC, &u1);
  merg_sort_rating_by_uid(filtered_ratings, filtered_rlength, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &u2);

  printf("euclidian sort uids in: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(u2.tv_sec - u1.tv_sec) + (double)(u2.tv_nsec - u1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  struct timespec e1, e2;
  clock_gettime(CLOCK_MONOTONIC, &e1);
  unsigned int simlen = euclidianscores(userid_a, ratings_a, numratings_a, uids, ulength, filtered_ratings, filtered_rlength, simscores);
  clock_gettime(CLOCK_MONOTONIC, &e2);

  printf("euclidianscores sim in: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(e2.tv_sec - e1.tv_sec) + (double)(e2.tv_nsec - e1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  struct timespec ws1, ws2;
  clock_gettime(CLOCK_MONOTONIC, &ws1);

  printf("notseen cnt: %d\n", notseen_cnt);
  struct weighted_score *wscores = malloc(notseen_cnt * sizeof(struct weighted_score));
  printf("simlen: %d\n", simlen);

  unsigned int ws_len = weighted_scores_short(simscores, ratings_notseen, simlen, notseen_cnt, wscores);

  clock_gettime(CLOCK_MONOTONIC, &ws2);
  printf("ws short: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(ws2.tv_sec - ws1.tv_sec) + (double)(ws2.tv_nsec - ws1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
  // printf("wslen?? %d\n", ws_len);

  struct timespec wsort1, wsort2;

  clock_gettime(CLOCK_MONOTONIC, &wsort1);
  merg_sort_ws_by_movid(wscores, ws_len, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &wsort2);

  printf("ws sorted in: %.17gms\n",
         ((double)(wsort2.tv_sec - wsort1.tv_sec) + (double)(wsort2.tv_nsec - wsort1.tv_nsec) / (double)1000000000L) * 1000);

  // struct movie_compact *filtered_movies_in_wscores =(sizeof(struct movie_compact) * mlength);

  int highest_id = 0;
  for (int i = 0; i < filtered_mlength; i++) {
    if (filtered_movies[i].movie_id > highest_id)
      highest_id = filtered_movies[i].movie_id;
  }

  printf("highest movid: %d\n", highest_id);

  int highest_numratings = 0;
  for (int i = 0; i < filtered_mlength; i++) {
    if (filtered_movies[i].num_ratings > highest_numratings)
      highest_numratings = filtered_movies[i].num_ratings;
  }
  printf("highest numratings: %d\n", highest_numratings);

  // int rows = highest_id;
  // int cols = highest_numratings + 1;

  struct movie_recommendation movie_recs[read_movies_lines()];

  unsigned int num_recs = 0;
  unsigned int is_mov_id = 0;
  unsigned int checked_wsind = 0;

  for (unsigned int i = 0; i < filtered_mlength; i++) {
    float weighted_score_sum = 0;
    float simscore_sum = 0;
    float recommendation_score = 0;

    for (unsigned int y = checked_wsind; y < ws_len; y++) {
      if (filtered_movies[i].movie_id == wscores[y].movie_id) {
        if (is_mov_id == 0) {
          is_mov_id = 1;
          checked_wsind = y;
        }
        weighted_score_sum += wscores[y].weighted_rating;
        simscore_sum += wscores[y].simscore;
      } else if (is_mov_id && filtered_movies[i].movie_id != wscores[y].movie_id) {
        is_mov_id = 0;
        break;
      }
    }

    recommendation_score = weighted_score_sum / simscore_sum;

    if (recommendation_score > 0.0) {
      movie_recs[num_recs].movie_id = filtered_movies[i].movie_id;
      movie_recs[num_recs].num_ratings = filtered_movies[i].num_ratings;
      movie_recs[num_recs].recommendation_score = recommendation_score;
      num_recs++;
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &ws2);
  printf("wscores: " YELLOW_OUTPUT "%.17gms\n",
         ((double)(ws2.tv_sec - ws1.tv_sec) + (double)(ws2.tv_nsec - ws1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  clock_gettime(CLOCK_MONOTONIC, &calc2);
  printf("calculations in: " YELLOW_OUTPUT "%0.17gms\n",
         ((double)(calc2.tv_sec - calc1.tv_sec) + (double)(calc2.tv_nsec - calc1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  merg_sort_movrec_by_rscore(movie_recs, num_recs, NUM_THREADS);

  clock_t numr1 = clock();

  bubble_sort_numr_rscore(movie_recs, num_recs);
  printf("sort numratings: %.17gms\n", ((float)(clock() - numr1) / CLOCKS_PER_SEC) * 1000);

  unsigned int num_print = 10;

  for (unsigned int i = 0; i < num_print; i++) {
    printf("movieid: %d, ratings: %d, score: %0.4f\n", movie_recs[i].movie_id, movie_recs[i].num_ratings, movie_recs[i].recommendation_score);
  }

  printf("total num recs: %d\n", num_recs);

  test_compare_movie_ids(movie_recs);
  test_check_duplicated_movie_ids(movie_recs, num_recs);
  test_compare_scores_diff(movie_recs, num_recs);
  test_compare_sim_scores(simscores);

  free(wscores);
  free(simscores);
  free(ratings_notseen);
  free(movseen_by_userid_a);
  free(filtered_ratings);
  free(filtered_movies);

  clock_gettime(CLOCK_MONOTONIC, &total2);
  printf("\ntotal recommendation time: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(total2.tv_sec - total1.tv_sec) + (double)(total2.tv_nsec - total1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
}

int main(void) {
  struct timespec r1, r2, f1, f2, loop_total1, loop_total2;
  clock_gettime(CLOCK_MONOTONIC, &loop_total1);

  unsigned int rating_file_size;
  struct rating *ratings;
  unsigned int rlength;

  unsigned int movie_file_size;
  struct movie *movies;
  unsigned int mlength;
  struct movie_title *mov_titles;

  // unsigned int *users;
  unsigned int *uids;
  unsigned int ulength;
  unsigned int user_num;

  clock_gettime(CLOCK_MONOTONIC, &r1);
  rating_file_size = read_ratings_lines();
  movie_file_size = read_movies_lines();
  clock_gettime(CLOCK_MONOTONIC, &r2);

  printf("num lines read in %.17gms\n", ((double)(r2.tv_sec - r1.tv_sec) + (double)(r2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  ratings = malloc(rating_file_size * sizeof(struct rating));
  rlength = read_ratings_fast(ratings);

  clock_gettime(CLOCK_MONOTONIC, &r2);
  printf("ratings read in %.17gms\n", ((double)(r2.tv_sec - r1.tv_sec) + (double)(r2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  movies = malloc((movie_file_size + 1) * sizeof(struct movie));
  mov_titles = malloc((movie_file_size + 1) * sizeof(struct movie_title));
  mlength = read_movies(movies, mov_titles);

  user_num = read_users_num(ratings, rlength);
  uids = malloc((user_num) * sizeof(unsigned int));

  ulength = read_users_from_ratings(uids, ratings, rlength);
  printf("ulength: %d\n", ulength);

  clock_gettime(CLOCK_MONOTONIC, &f2);
  printf("files read in %.17gms\n", ((double)(f2.tv_sec - r1.tv_sec) + (double)(f2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  unsigned int num_loops = 1;
  for (unsigned int i = 0; i < num_loops; i++)
    get_recommendations(movies, ratings, uids, mlength, rlength, ulength);

  clock_gettime(CLOCK_MONOTONIC, &loop_total2);

  printf("total rec loops in %.17gms, avg: %.17fms\n",
         ((double)(loop_total2.tv_sec - loop_total1.tv_sec) + (double)(loop_total2.tv_nsec - loop_total1.tv_nsec) / (double)1000000000L) * 1000,
         (((double)(loop_total2.tv_sec - loop_total1.tv_sec) + (double)(loop_total2.tv_nsec - loop_total1.tv_nsec) / (double)1000000000L) * 1000) /
             num_loops);

  free(ratings);
  free(movies);
  free(mov_titles);
  free(uids);

  return 0;
}
