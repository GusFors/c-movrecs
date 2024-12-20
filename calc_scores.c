#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "array_sorting.h"
#include <pthread.h>
#include "regression_tests.h"

unsigned int weighted_scores_short(struct user_sim *simscores, struct rating *ratings_notseen, unsigned int simlen, unsigned int ratings_notseen_length,
                                   struct weighted_score *wscores) {
  struct timespec t1, t2;
  clock_gettime(CLOCK_MONOTONIC, &t1);

  int is_sorted = 1;
  for (unsigned int i = 0; i < ratings_notseen_length - 1; i++) {
    if (ratings_notseen[i].user_id > ratings_notseen[i + 1].user_id)
      is_sorted = 0;
  }

  if (!is_sorted) {
    PRINT_VERBOSE("INFO: weighted_scores_short, ratings not sorted by user_id, sorting before continuing\n");
    merg_sort_rating_by_uid(ratings_notseen, ratings_notseen_length, NUM_THREADS);
  }

  clock_gettime(CLOCK_MONOTONIC, &t2);

  printf("rating by uid time: " YELLOW_OUTPUT "%0.17f" RESET_OUTPUT "\n",
         ((double)(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_nsec - t1.tv_nsec) / (double)1000000000L) * 1000);
  printf(RESET_OUTPUT);

  unsigned int ws_len = 0;
  unsigned int counted_indexes = 0;
  unsigned int is_curr_id = 0;

  for (unsigned int i = 0, l = 0; i < simlen; i++) {
    for (unsigned int j = counted_indexes; j < ratings_notseen_length; j++) {
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

unsigned int euclidianscores_match(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids, unsigned int ulength,
                                   struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores) {
  unsigned int userid_b = 0;
  unsigned int simlen = 0;
  unsigned int checked_rindex = 0;
  unsigned int highest_movid = 0;

  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].movie_id > highest_movid)
      highest_movid = filtered_ratings[i].movie_id;
  }

  struct rating *matching_ratings = calloc(highest_movid, sizeof(struct rating));

  for (unsigned int i = 0; i < numratings_a; i++) {
    matching_ratings[ratings_a[i].movie_id] = ratings_a[i];
  }

  struct rating *ratings_b = calloc(highest_movid, sizeof(struct rating));
  unsigned int ratings_b_length = 0;
  unsigned int used_index[65536] = {0};

  for (unsigned int i = 0; i < ulength; i++) {
    unsigned int is_curr_user = 0;
    userid_b = uids[i];

    for (unsigned int j = 0; j < ratings_b_length; j++) {
      ratings_b[used_index[j]].movie_id = 0;
      ratings_b[used_index[j]].user_id = 0;
      ratings_b[used_index[j]].rating = 0.0;
    }

    ratings_b_length = 0;

    float sim = 0;
    unsigned int n = 0;

    for (unsigned int j = checked_rindex; j < filtered_rlength; j++) {
      if (filtered_ratings[j].user_id == userid_b) {

        // matching_ratings[filtered_ratings[j].movie_id].movie_id == filtered_ratings[j].movie_id
        if (matching_ratings[filtered_ratings[j].movie_id].movie_id > 0) {
          ratings_b[filtered_ratings[j].movie_id] = filtered_ratings[j];
          used_index[ratings_b_length] = filtered_ratings[j].movie_id;
          ratings_b_length++;
        }

        if (is_curr_user == 0) {
          is_curr_user = 1;
        }

      } else if (is_curr_user && (filtered_ratings[j].user_id != userid_b)) {
        is_curr_user = 0;
        checked_rindex = j;
        break;
      }
    }

    unsigned int current_movid_index = 0;
    for (unsigned int j = 0; j < numratings_a; j++) {
      current_movid_index = ratings_a[j].movie_id;

      if (ratings_b[current_movid_index].movie_id > 0) {
        n += 1;
        sim += (((matching_ratings[current_movid_index].rating) - ratings_b[current_movid_index].rating) *
                ((matching_ratings[current_movid_index].rating) - ratings_b[current_movid_index].rating));
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

  free(matching_ratings);
  free(ratings_b);

  return simlen;
}

unsigned int euclidianscores_short(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids, unsigned int ulength,
                                   struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores) {
  unsigned int userid_b = 0;
  unsigned int simlen = 0;
  unsigned int checked_rindex = 0;

  for (unsigned int i = 0; i < ulength; i++) {
    unsigned int is_curr_user = 0;
    userid_b = uids[i];
    struct rating ratings_b[32768 * 2];
    unsigned int ratings_b_length = 0;

    float sim = 0;
    unsigned int n = 0;

    for (unsigned int j = checked_rindex; j < filtered_rlength; j++) {
      if (filtered_ratings[j].user_id == userid_b) {
        ratings_b[ratings_b_length] = filtered_ratings[j];
        ratings_b_length++;

        if (is_curr_user == 0) {
          is_curr_user = 1;
        }
      } else if (is_curr_user && (filtered_ratings[j].user_id != userid_b)) {
        is_curr_user = 0;
        checked_rindex = j;
        break;
      }
    }

    unsigned int checked_userb_ratings = 0;

    for (unsigned int i = 0; i < ratings_b_length; i++) {
      for (unsigned int j = checked_userb_ratings; j < numratings_a; j++) {
        if (ratings_a[j].movie_id == ratings_b[i].movie_id) {
          sim += ((ratings_a[j].rating - ratings_b[i].rating) * (ratings_a[j].rating - ratings_b[i].rating));
          n += 1;
          checked_userb_ratings++;
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
