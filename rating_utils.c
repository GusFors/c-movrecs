#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "rating_utils.h"

void calc_num_ratings(struct movie *movies, struct rating *ratings, int mlength, int rlength) {
  clock_t t1 = clock();

  int is_curr_mov_id = 0;
  int already_checked_indexes = 0;

  for (int i = 0; i < mlength; i++) {
    int num_mratings = 0;
    for (int y = already_checked_indexes; y < rlength; y++) {
      if (movies[i].movie_id == ratings[y].movie_id) {
        if (is_curr_mov_id == 0) {
          is_curr_mov_id = 1;
          already_checked_indexes = y;
        }
        num_mratings++;
      } else if (is_curr_mov_id && movies[i].movie_id != ratings[y].movie_id) {
        is_curr_mov_id = 0;
        break;
      }
    }
    movies[i].num_ratings = num_mratings;
  }

  double total = ((double)clock() - t1) / CLOCKS_PER_SEC;
  printf("calc num_ratings in:   %.17gms\n", total * 1000);
}

int filter_numratings(struct movie *movies, struct rating *ratings, int mlength, int rlength, int min_numratings, struct rating *filtered_ratings) {
  clock_t t1 = clock();

  int filtered_rlength = 0;

  int is_curr_mov = 0;
  int checked_indexes = 0;

  for (int i = 0; i < mlength; i++) {
    for (int y = checked_indexes; y < rlength; y++) {
      if (movies[i].movie_id == ratings[y].movie_id) {
        if (is_curr_mov == 0) {
          is_curr_mov = 1;
          checked_indexes = y;
        }

        if (movies[i].num_ratings >= min_numratings) {
          filtered_ratings[filtered_rlength] = ratings[y];
          filtered_rlength++;
        }

      } else if (is_curr_mov && movies[i].movie_id != ratings[y].movie_id) {
        is_curr_mov = 0;
        break;
      }
    }
  }

  double total = ((double)clock() - t1) / CLOCKS_PER_SEC;
  printf("filter num_ratings in: %.17gms\n", total * 1000);
  return filtered_rlength;
}

int filter_movie_numratings(struct movie *movies, int mlength, int min_numratings, struct movie_compact *filtered_movies) {
  clock_t t1 = clock();

  int filtered_mlength = 0;

  int is_curr_mov = 0;
  int checked_indexes = 0;

  for (int i = 0; i < mlength; i++) {
    if (movies[i].num_ratings >= min_numratings) {
      filtered_movies[filtered_mlength].movie_id = movies[i].movie_id;
      filtered_movies[filtered_mlength].num_ratings = movies[i].num_ratings;
      filtered_mlength++;
    }
  }

  double total = ((double)clock() - t1) / CLOCKS_PER_SEC;
  printf("filter movie num_ratings in: %.17gms\n", total * 1000);
  return filtered_mlength;
}

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