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

  printf("rating by uid time: " YELLOW_OUTPUT "%0.17f\n", ((double)(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_nsec - t1.tv_nsec) / (double)1000000000L) * 1000);
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
      simscores[simlen].user_id = userid_b;
      simscores[simlen].simscore = inv;
      simlen++;
    }
  }

  return simlen;
}

void get_recommendations(struct movie *movies, struct rating *ratings, unsigned int *uids, unsigned int mlength, unsigned int rlength, unsigned int ulength) {
  struct timespec total1, total2;

  clock_gettime(CLOCK_MONOTONIC, &total1);

  clock_t s1 = clock();
  struct timespec calc1, calc2;

  clock_gettime(CLOCK_MONOTONIC, &calc1);
  merg_sort_rating_by_movid(ratings, rlength, NUM_THREADS);
  printf("sort in  %.17gms\n", ((float)(clock() - s1) / CLOCKS_PER_SEC) * 1000);

  calc_num_ratings(movies, ratings, mlength, rlength);

  unsigned int min_numratings = 3;
  struct rating *filtered_ratings = malloc(sizeof(struct rating) * rlength);
  struct movie_compact *filtered_movies = malloc(sizeof(struct movie_compact) * mlength);

  unsigned int filtered_mlength = filter_movie_numratings(movies, mlength, min_numratings, filtered_movies);
  unsigned int filtered_rlength = filter_numratings(movies, ratings, mlength, rlength, min_numratings, filtered_ratings);

  clock_t c1 = clock();

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

  printf("user a ratings in %.17gms\n", ((float)(clock() - c1) / CLOCKS_PER_SEC) * 1000);

  clock_t r1 = clock();
  struct rating ratings_a[numratings_a];

  printf("rlength: %d\n", filtered_rlength);

  for (unsigned int i = 0, y = 0, j = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      ratings_a[y] = filtered_ratings[i];
      y++;
    }
  }

  printf("r1 in %.17gms\n", ((float)(clock() - r1) / CLOCKS_PER_SEC) * 1000);

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

  clock_t u1 = clock();
  unsigned int userlen = ulength - 1;
  struct user_sim *simscores = malloc(userlen * sizeof(struct user_sim));

  printf("ulength: %d\n", ulength);

  merg_sort_rating_by_uid(filtered_ratings, filtered_rlength, NUM_THREADS);
  // merge_sort_thread_handler(filtered_ratings, filtered_rlength, NUM_THREADS, (void *)&merg_sort_rating_by_uid, (void *)&merg_sort_merge_uid);

  clock_t e1 = clock();
  unsigned int simlen = euclidianscores(userid_a, ratings_a, numratings_a, uids, ulength, filtered_ratings, filtered_rlength, simscores);

  printf("euclidianscores sim: " YELLOW_OUTPUT "%.17gms\n", ((float)(clock() - e1) / CLOCKS_PER_SEC) * 1000);
  printf(RESET_OUTPUT);
  printf("euclidianscores total: " YELLOW_OUTPUT "%.17gms\n", ((float)(clock() - u1) / CLOCKS_PER_SEC) * 1000);
  printf(RESET_OUTPUT);

  clock_t w1 = clock();
  printf("notseen cnt: %d\n", notseen_cnt);
  struct weighted_score *wscores = malloc(notseen_cnt * sizeof(struct weighted_score));
  printf("simlen: %d\n", simlen);

  unsigned int ws_len = weighted_scores_short(simscores, ratings_notseen, simlen, notseen_cnt, wscores);

  printf("ws short: %.17gms\n", ((float)(clock() - w1) / CLOCKS_PER_SEC) * 1000);
  printf("wslen?? %d\n", ws_len);

  clock_t wsort1 = clock();
  merg_sort_ws_by_movid(wscores, ws_len, NUM_THREADS);
  printf("ws sorted in: %.17gms\n", ((float)(clock() - wsort1) / CLOCKS_PER_SEC) * 1000);

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

  printf("wscores: " YELLOW_OUTPUT "%.17gms\n", ((float)(clock() - w1) / CLOCKS_PER_SEC) * 1000);
  printf(RESET_OUTPUT);

  clock_gettime(CLOCK_MONOTONIC, &calc2);
  printf("calculations in: " YELLOW_OUTPUT "%0.17gms\n", ((double)(calc2.tv_sec - calc1.tv_sec) + (double)(calc2.tv_nsec - calc1.tv_nsec) / (double)1000000000L) * 1000);
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

  free(wscores);
  free(simscores);
  free(ratings_notseen);
  free(movseen_by_userid_a);
  free(filtered_ratings);
  free(filtered_movies);

  clock_gettime(CLOCK_MONOTONIC, &total2);
  printf("total recommendation time: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(total2.tv_sec - total1.tv_sec) + (double)(total2.tv_nsec - total1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
}

int main(void) {
  clock_t t1 = clock();

  unsigned int rating_file_size;
  struct rating *ratings;
  unsigned int rlength;

  unsigned int movie_file_size;
  struct movie *movies;
  unsigned int mlength;

  // unsigned int *users;
  unsigned int *uids;
  unsigned int ulength;
  unsigned int user_num;

  clock_t rating_read1 = clock();

  rating_file_size = read_ratings_lines();
  movie_file_size = read_movies_lines();

  printf("num lines read in %.17gms\n", ((float)(clock() - t1) / CLOCKS_PER_SEC) * 1000);

  ratings = malloc(rating_file_size * sizeof(struct rating));
  rlength = read_ratings_fast(ratings);

  printf("ratings read in %.17gms\n", ((float)(clock() - rating_read1) / CLOCKS_PER_SEC) * 1000);

  movies = malloc((movie_file_size + 1) * sizeof(struct movie));
  mlength = read_movies(movies);

  user_num = read_users_num(ratings, rlength);
  uids = malloc((user_num) * sizeof(unsigned int));

  ulength = read_users_from_ratings(uids, ratings, rlength);
  printf("ulength: %d\n", ulength);

  printf("files read in %.17gms\n", ((float)(clock() - t1) / CLOCKS_PER_SEC) * 1000);

  unsigned int num_loops = 1;
  clock_t total1 = clock();

  for (unsigned int i = 0; i < num_loops; i++)
    get_recommendations(movies, ratings, uids, mlength, rlength, ulength);

  clock_t total2 = clock() - total1;
  float total = ((float)total2) / CLOCKS_PER_SEC;
  printf("total rec loops in %.17gms, avg: %.17fms\n", ((float)(clock() - total1) / CLOCKS_PER_SEC) * 1000, (total * 1000) / num_loops);

  free(ratings);
  free(movies);
  free(uids);

  return 0;
}
