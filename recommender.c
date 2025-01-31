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
#include <stdint.h>

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

  unsigned int highest_movid = 0;
  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].movie_id > highest_movid)
      highest_movid = filtered_ratings[i].movie_id;
  }

  unsigned int *matching_ratings_a = calloc(highest_movid + 1, sizeof(unsigned int)); // +1 to not read outside array bounds
  PRINT_VERBOSE("highest movid: %d\n", highest_movid);

  unsigned int numratings_a = count_user_ratings(userid_a, filtered_ratings, filtered_rlength);
  unsigned int *movseen_by_userid_a = malloc(numratings_a * sizeof(unsigned int));

  for (unsigned int i = 0, y = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a) {
      movseen_by_userid_a[y] = filtered_ratings[i].movie_id;
      matching_ratings_a[filtered_ratings[i].movie_id] = 1;
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

  unsigned int ratings_notseen_length = 0; //
  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (matching_ratings_a[filtered_ratings[i].movie_id] == 0)
      ratings_notseen_length++;
  }

  struct rating *ratings_notseen = malloc(ratings_notseen_length * sizeof(struct rating));
  printf("ratings notseen length: %d\n", ratings_notseen_length);

  unsigned int ratings_notseen_length2 = 0;
  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (matching_ratings_a[filtered_ratings[i].movie_id] == 0) {
      ratings_notseen[ratings_notseen_length2] = filtered_ratings[i];
      ratings_notseen_length2++;
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

  PRINT_VERBOSE("notseen cnt: %d\n", ratings_notseen_length);
  struct weighted_score *wscores = malloc(ratings_notseen_length * sizeof(struct weighted_score));
  PRINT_VERBOSE("simlen: %d\n", simlen);

  unsigned int ws_len = weighted_scores_short(simscores, ratings_notseen, simlen, ratings_notseen_length, wscores);
  // unsigned int ws_len = weighted_scores_short(simscores, filtered_ratings, simlen, filtered_rlength, wscores);

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

  clock_gettime(CLOCK_MONOTONIC, &ws1);

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

  clock_gettime(CLOCK_MONOTONIC, &ws2);
  printf("movie recommendation scores: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(ws2.tv_sec - ws1.tv_sec) + (double)(ws2.tv_nsec - ws1.tv_nsec) / (double)1000000000L) * 1000);
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

  unsigned int num_print = 7;

  for (unsigned int i = 0; i < num_print && i < num_recs; i++) {
    printf("movieid: %d, ratings: %d, score: %0.4f\n", movie_recs[i].movie_id, movie_recs[i].num_ratings, movie_recs[i].recommendation_score);
  }

  printf("total num recs: %d\n", num_recs);

  free(wscores);
  free(simscores);
  free(ratings_notseen);
  free(movseen_by_userid_a);
  free(filtered_movies);
  free(matching_ratings_a);
  if ((flags & RATINGS_EXTRA_FILTER))
    free(filtered_ratings);

  clock_gettime(CLOCK_MONOTONIC, &total2);
  printf("\ntotal recommendation time: " YELLOW_OUTPUT "%0.17f\n",
         ((double)(total2.tv_sec - total1.tv_sec) + (double)(total2.tv_nsec - total1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);
}
