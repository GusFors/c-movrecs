#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "sorting.h"
#include "readfiles.h"
#include "rating_utils.h"
#include <pthread.h>
#include "regression_tests.h"
#include "calc_scores.h"

void get_recommendations(struct movie *movies, struct rating *ratings, unsigned int *uids, unsigned int mlength, unsigned int rlength,
                         unsigned int ulength, unsigned int flags) {
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
  clock_t w1 = clock();

  printf("notseen cnt: %d\n", notseen_cnt);
  struct weighted_score *wscores = malloc(notseen_cnt * sizeof(struct weighted_score));
  printf("simlen: %d\n", simlen);

  unsigned int ws_len = weighted_scores_short(simscores, ratings_notseen, simlen, notseen_cnt, wscores);

  clock_gettime(CLOCK_MONOTONIC, &ws2);
  printf("ws short: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(ws2.tv_sec - ws1.tv_sec) + (double)(ws2.tv_nsec - ws1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
  // printf("wslen?? %d\n", ws_len);

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

  if (flags & TESTS) {
    test_compare_movie_ids(movie_recs);
    test_check_duplicated_movie_ids(movie_recs, num_recs);
    test_compare_scores_diff(movie_recs, num_recs);
    test_compare_sim_scores(simscores);
  }

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
