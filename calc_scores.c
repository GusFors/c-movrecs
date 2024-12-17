#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "array_sorting.h"
#include <pthread.h>
#include "regression_tests.h"

unsigned int weighted_scores_short(struct user_sim *simscores, struct rating *ratings_notseen, unsigned int simlen, unsigned int notseen_cnt,
                                   struct weighted_score *wscores) {
  struct timespec t1, t2;
  clock_gettime(CLOCK_MONOTONIC, &t1);

  int is_sorted = 1;
  for (unsigned int i = 0; i < notseen_cnt - 1; i++) {
    if (ratings_notseen[i].user_id > ratings_notseen[i + 1].user_id)
      is_sorted = 0;
  }

  if (!is_sorted) {
    PRINT_VERBOSE("INFO: weighted_scores_short, ratings not sorted by user_id, sorting before continuing\n");
    merg_sort_rating_by_uid(ratings_notseen, notseen_cnt, NUM_THREADS);
  }

  clock_gettime(CLOCK_MONOTONIC, &t2);

  printf("rating by uid time: " YELLOW_OUTPUT "%0.17f" RESET_OUTPUT "\n",
         ((double)(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_nsec - t1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  unsigned int ws_len = 0;
  unsigned int counted_indexes = 0;
  unsigned int is_curr_id = 0;

  for (unsigned int i = 0, l = 0; i < simlen; i++) {
    for (unsigned int j = counted_indexes; j < notseen_cnt; j++) {
      if (simscores[i].user_id == ratings_notseen[j].user_id) {

        if (is_curr_id == 0)
          is_curr_id = 1;

        wscores[l].movie_id = ratings_notseen[j].movie_id;
        wscores[l].simscore = simscores[i].simscore;
        wscores[l].weighted_rating = (simscores[i].simscore * ratings_notseen[j].rating);

        l++;
        ws_len++;
      } else if (is_curr_id && simscores[i].user_id != ratings_notseen[j].user_id) {
        is_curr_id = 0;
        counted_indexes = j;
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

  for (unsigned int i = 0; i < ulength; i++) {
    unsigned int is_curr_user = 0;
    userid_b = uids[i];

    float sim = 0;
    unsigned int n = 0;

    for (unsigned int j = checked_rindex; j < filtered_rlength; j++) {
      if (filtered_ratings[j].user_id == userid_b) {
        if (is_curr_user == 0) {
          is_curr_user = 1;
        }
      } else if (is_curr_user && (filtered_ratings[j].user_id != userid_b)) {
        is_curr_user = 0;
        checked_rindex = j;
        break;
      }

      for (unsigned int k = 0; k < numratings_a; k++) {
        if (filtered_ratings[j].movie_id == ratings_a[k].movie_id) {
          sim += ((ratings_a[k].rating - filtered_ratings[j].rating) * (ratings_a[k].rating - filtered_ratings[j].rating));
          n += 1;
        }
      }
    }

    if (uids[i] == userid_a)
      continue;

    if (n == 0)
      continue;

    float inv = 1.0f / (1.0f + sim);

    if (inv > 0.0f) {
      simscores[simlen].user_id = userid_b;
      simscores[simlen].simscore = inv;
      simlen++;
    }
  }

  return simlen;
}
