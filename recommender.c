#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "array_sorting.h"
#include "readfiles.h"
#include "rating_utils.h"
#include <pthread.h>
#include "regression_tests.h"
#include "calc_scores.h"

void get_recommendations(unsigned int userid_a, unsigned int min_numratings, struct movie *movies, struct rating *ratings, unsigned int *uids,
                         unsigned int mlength, unsigned int rlength, unsigned int ulength, unsigned int flags) {
  printf("Calculating recommendations for user: %d, min_numratings: %d\n", userid_a, min_numratings);

  struct timespec total1, total2;
  clock_gettime(CLOCK_MONOTONIC, &total1);

  struct timespec calc1, calc2, sort1;

  clock_gettime(CLOCK_MONOTONIC, &calc1);
  merg_sort_rating_by_movid(ratings, rlength, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &sort1);

  PRINT_VERBOSE("sort ratings by movid in: " YELLOW_OUTPUT "%0.17f" RESET_OUTPUT "\n",
                ((double)(sort1.tv_sec - calc1.tv_sec) + (double)(sort1.tv_nsec - calc1.tv_nsec) / (double)1000000000L) * 1000);

  calc_num_ratings(movies, ratings, mlength, rlength);

  struct rating *filtered_ratings;
  struct movie_compact *filtered_movies = malloc(sizeof(struct movie_compact) * mlength);

  unsigned int filtered_mlength = filter_movie_numratings(movies, mlength, min_numratings, filtered_movies);
  unsigned int filtered_rlength = rlength;
  filtered_ratings = ratings;

  if (flags & RATINGS_EXTRA_FILTER) {
    filtered_ratings = malloc(sizeof(struct rating) * rlength);
    filtered_rlength = filter_numratings(movies, ratings, mlength, rlength, min_numratings, filtered_ratings);
  }

  clock_t c1 = clock();

  unsigned int numratings_a = count_user_ratings(userid_a, filtered_ratings, filtered_rlength);
  unsigned int *movseen_by_userid_a = malloc(numratings_a * sizeof(unsigned int));

  for (unsigned int i = 0, y = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      movseen_by_userid_a[y] = filtered_ratings[i].movie_id;
      y++;
    }
  }

  PRINT_VERBOSE("movseen num by a: %d\n", numratings_a);
  printf("user a ratings in %.17gms\n", ((float)(clock() - c1) / CLOCKS_PER_SEC) * 1000);

  clock_t r1 = clock();
  struct rating ratings_a[numratings_a];

  PRINT_VERBOSE("rlength: %d\n", filtered_rlength);

  for (unsigned int i = 0, y = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      ratings_a[y] = filtered_ratings[i];
      y++;
    }
  }

  printf("r1 in %.17gms\n", ((float)(clock() - r1) / CLOCKS_PER_SEC) * 1000);
  merg_sort_rating_by_uid(filtered_ratings, filtered_rlength, NUM_THREADS);

  clock_t notseen_t1 = clock();
  unsigned int notseen_cnt = 0;
  for (unsigned int i = 0; i < filtered_rlength; i++) {
    unsigned int isseen = 0;
    for (unsigned int j = 0; j < numratings_a; j++) {
      if (movseen_by_userid_a[j] == filtered_ratings[i].movie_id) {
        isseen = 1;
      }
    }
    if (!isseen)
      notseen_cnt++;
  }

  struct rating *ratings_notseen = malloc(notseen_cnt * sizeof(struct rating));

  unsigned int notseen_cnt2 = 0;
  for (unsigned int i = 0; i < filtered_rlength; i++) {
    unsigned int isseen = 0;
    for (unsigned int j = 0; j < numratings_a; j++) {
      if (movseen_by_userid_a[j] == filtered_ratings[i].movie_id) {
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
  PRINT_VERBOSE("ulength: %d\n", ulength);

  struct timespec u1, u2;
  clock_gettime(CLOCK_MONOTONIC, &u1);
  // merg_sort_rating_by_uid(filtered_ratings, filtered_rlength, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &u2);

  printf("euclidian sort uids in: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(u2.tv_sec - u1.tv_sec) + (double)(u2.tv_nsec - u1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  struct timespec e1, e2;
  clock_gettime(CLOCK_MONOTONIC, &e1);
  unsigned int simlen = euclidianscores_match(userid_a, ratings_a, numratings_a, uids, ulength, filtered_ratings, filtered_rlength, simscores);
  clock_gettime(CLOCK_MONOTONIC, &e2);

  printf("euclidianscores sim in: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(e2.tv_sec - e1.tv_sec) + (double)(e2.tv_nsec - e1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  struct timespec ws1, ws2;

  clock_gettime(CLOCK_MONOTONIC, &ws1);
  clock_t w1 = clock();

  PRINT_VERBOSE("notseen cnt: %d\n", notseen_cnt);
  struct weighted_score *wscores = malloc(notseen_cnt * sizeof(struct weighted_score));
  PRINT_VERBOSE("simlen: %d\n", simlen);

  unsigned int ws_len = weighted_scores_short(simscores, ratings_notseen, simlen, notseen_cnt, wscores);

  clock_gettime(CLOCK_MONOTONIC, &ws2);
  printf("ws short: " YELLOW_OUTPUT "%0.17f\n", ((double)(ws2.tv_sec - ws1.tv_sec) + (double)(ws2.tv_nsec - ws1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  struct timespec wsort1, wsort2;

  clock_gettime(CLOCK_MONOTONIC, &wsort1);
  merg_sort_ws_by_movid(wscores, ws_len, NUM_THREADS);
  clock_gettime(CLOCK_MONOTONIC, &wsort2);

  printf("ws sorted in: " YELLOW_OUTPUT "%0.17f" RESET_OUTPUT "\n",
         ((double)(wsort2.tv_sec - wsort1.tv_sec) + (double)(wsort2.tv_nsec - wsort1.tv_nsec) / (double)1000000000L) * 1000);

  // struct movie_compact *filtered_movies_in_wscores =(sizeof(struct movie_compact) * mlength);

  unsigned int highest_id = 0;
  for (unsigned int i = 0; i < filtered_mlength; i++) {
    if (filtered_movies[i].movie_id > highest_id)
      highest_id = filtered_movies[i].movie_id;
  }

  PRINT_VERBOSE("highest movid: %d\n", highest_id);

  unsigned int highest_numratings = 0;
  for (unsigned int i = 0; i < filtered_mlength; i++) {
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

    for (unsigned int j = checked_wsind; j < ws_len; j++) {
      if (filtered_movies[i].movie_id == wscores[j].movie_id) {
        if (is_mov_id == 0) {
          is_mov_id = 1;
          checked_wsind = j;
        }
        weighted_score_sum += wscores[j].weighted_rating;
        simscore_sum += wscores[j].simscore;
      } else if (is_mov_id && filtered_movies[i].movie_id != wscores[j].movie_id) {
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

  if (flags & TESTS) {
    test_check_duplicated_movie_ids(movie_recs, num_recs);
    // test_compare_scores_diff(movie_recs, num_recs);
    test_compare_sim_scores(simscores, simlen, userid_a);
    test_movie_recommendation_tests(movie_recs, num_recs, userid_a);
  }

  merg_sort_movrec_by_rscore(movie_recs, num_recs, NUM_THREADS);

  clock_t numr1 = clock();

  bubble_sort_numr_rscore(movie_recs, num_recs);
  printf("sort numratings: %.17gms\n", ((float)(clock() - numr1) / CLOCKS_PER_SEC) * 1000);

  unsigned int num_print = 16;

  for (unsigned int i = 0; i < num_print && i < num_recs; i++) {
    printf("movieid: %d, ratings: %d, score: %0.4f\n", movie_recs[i].movie_id, movie_recs[i].num_ratings, movie_recs[i].recommendation_score);
  }

  printf("total num recs: %d\n", num_recs);

  free(wscores);
  free(simscores);
  free(ratings_notseen);
  free(movseen_by_userid_a);
  free(filtered_movies);
  if ((flags & RATINGS_EXTRA_FILTER))
    free(filtered_ratings);

  clock_gettime(CLOCK_MONOTONIC, &total2);
  printf("\ntotal recommendation time: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(total2.tv_sec - total1.tv_sec) + (double)(total2.tv_nsec - total1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
}
