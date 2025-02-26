#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "rating_utils.h"
#include "array_sorting.h"
#include "regression_tests.h"

static unsigned int cached_num_ratings = 0;

// add arg/macro to force calc
void calc_num_ratings(struct movie *movies, struct rating *ratings, unsigned int mlength, unsigned int rlength) {
  struct timespec sort1, sort2;
  clock_t t1 = clock();

  if (!cached_num_ratings) {
    clock_gettime(CLOCK_MONOTONIC, &sort1);

    unsigned int is_sorted = 1;
    for (unsigned int i = 0; i < rlength - 1; i++) {
      if (ratings[i].movie_id > ratings[i + 1].movie_id)
        is_sorted = 0;
    }

    if (!is_sorted) {
      PRINT_VERBOSE("INFO: calc_num_ratings: ratings not sorted by movie_id, sorting before continuing\n");
      merg_sort_rating_by_movid(ratings, rlength, NUM_THREADS);
    }

    clock_gettime(CLOCK_MONOTONIC, &sort2);

    PRINT_VERBOSE("sort ratings by movid in: " YELLOW_OUTPUT "%0.17f" RESET_OUTPUT "\n",
                  ((double)(sort2.tv_sec - sort1.tv_sec) + (double)(sort2.tv_nsec - sort1.tv_nsec) / (double)1000000000L) * 1000);

    unsigned int is_curr_mov_id = 0;
    unsigned int already_checked_indexes = 0;

    for (unsigned int i = 0; i < mlength; i++) {
      unsigned int num_mratings = 0;
      for (unsigned int j = already_checked_indexes; j < rlength; j++) {
        if (movies[i].movie_id == ratings[j].movie_id) {
          if (is_curr_mov_id == 0) {
            is_curr_mov_id = 1;
            already_checked_indexes = j;
          }
          num_mratings++;
        } else if (is_curr_mov_id && movies[i].movie_id != ratings[j].movie_id) {
          is_curr_mov_id = 0;
          break;
        }
      }
      // keep separate array with only num ratings to use when filter by numratings later? then checking matching movie id can be skipped
      movies[i].num_ratings = num_mratings;
    }
    cached_num_ratings = 1;
  }

  double total = ((double)clock() - (double)t1) / CLOCKS_PER_SEC;
  printf("calc num_ratings in:   %.17gms\n", total * 1000);
}

unsigned int filter_numratings(struct movie *movies, struct rating *ratings, unsigned int mlength, unsigned int rlength, unsigned int min_numratings,
                               struct rating *filtered_ratings) {
  clock_t t1 = clock();

  unsigned int filtered_rlength = 0;
  unsigned int is_curr_mov = 0;
  unsigned int checked_indexes = 0;

  for (unsigned int i = 0; i < mlength; i++) {
    for (unsigned int j = checked_indexes; j < rlength; j++) {
      if (movies[i].movie_id == ratings[j].movie_id) {
        if (is_curr_mov == 0) {
          is_curr_mov = 1;
          checked_indexes = j;
        }

        if (movies[i].num_ratings >= min_numratings) {
          filtered_ratings[filtered_rlength] = ratings[j];
          filtered_rlength++;
        }

      } else if (is_curr_mov && movies[i].movie_id != ratings[j].movie_id) {
        is_curr_mov = 0;
        break;
      }
    }
  }

  double total = ((double)clock() - (double)t1) / CLOCKS_PER_SEC;
  printf("filter num_ratings from %d, to: %d, in: %.17gms\n", rlength, filtered_rlength, total * 1000);
  return filtered_rlength;
}

unsigned int filter_movie_numratings(struct movie *movies, unsigned int mlength, unsigned int min_numratings, struct movie_compact *filtered_movies) {
  clock_t t1 = clock();

  unsigned int filtered_mlength = 0;

  for (unsigned int i = 0; i < mlength; i++) {
    if (movies[i].num_ratings >= min_numratings) {
      filtered_movies[filtered_mlength].movie_id = movies[i].movie_id;
      filtered_movies[filtered_mlength].num_ratings = movies[i].num_ratings;
      filtered_mlength++;
    }
  }

  double total = ((double)clock() - (double)t1) / CLOCKS_PER_SEC;
  printf("filter movie num_ratings from: %d, to: %d in: %.17gms\n", mlength, filtered_mlength, total * 1000);
  return filtered_mlength;
}

unsigned int count_user_ratings(unsigned int userid_a, struct rating *filtered_ratings, unsigned int filtered_rlength) {
  unsigned int numratings_a = 0;
  // unsigned int numratings_other = 0;

  for (unsigned int i = 0; i < filtered_rlength; i++) {
    if (filtered_ratings[i].user_id == userid_a)
      numratings_a++;
    // else
    //   numratings_other++;
  }

  return numratings_a;
}
