#include <bits/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "readfiles.h"
#include <pthread.h>

int main(int argc, char *argv[]) {
  unsigned int flags = 0b0000;

  for (int i = 1; i < argc; i++) {
    printf("arg[%d]: %s\n", i, argv[i]);
    if (strncmp(argv[i], "-t", sizeof("-t")) == 0)
      flags |= TESTS;

    if (strncmp(argv[i], "-v", sizeof("-s")) == 0)
      flags |= VERBOSE_DEBUG;

    if (strncmp(argv[i], "-s", sizeof("-s")) == 0)
      flags |= SINGLE_THREADED;
  }

  printf("flags: %d\n", flags);

  struct timespec r1, r2, f2, loop_total1, loop_total2;
  clock_gettime(CLOCK_MONOTONIC, &loop_total1);

  ssize_t rating_file_size;
  struct rating *ratings;
  ssize_t rlength;

  ssize_t movie_file_size;
  struct movie *movies;
  struct movie_title *mov_titles;
  ssize_t mlength;

  // unsigned int *users;
  unsigned int *uids;
  size_t ulength;
  size_t user_num;

  clock_gettime(CLOCK_MONOTONIC, &r1);
  rating_file_size = read_ratings_lines();
  movie_file_size = read_movies_lines();
  clock_gettime(CLOCK_MONOTONIC, &r2);

  printf("num lines read in %.17gms\n", ((double)(r2.tv_sec - r1.tv_sec) + (double)(r2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  ratings = malloc((unsigned)rating_file_size * sizeof(struct rating));
  rlength = read_ratings_fast(ratings);

  clock_gettime(CLOCK_MONOTONIC, &r2);
  printf("ratings read in %.17gms\n", ((double)(r2.tv_sec - r1.tv_sec) + (double)(r2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  movies = malloc(((size_t)movie_file_size + 1) * sizeof(struct movie));
  mov_titles = malloc(((size_t)movie_file_size + 1) * sizeof(struct movie_title));
  mlength = read_movies(movies, mov_titles);

  user_num = read_users_num(ratings, (unsigned)rlength);
  uids = malloc((user_num) * sizeof(unsigned int));

  ulength = read_users_from_ratings(uids, ratings, (unsigned)rlength);
  printf("ulength: %zd\n", ulength);

  clock_gettime(CLOCK_MONOTONIC, &f2);
  printf("files read in %.17gms\n", ((double)(f2.tv_sec - r1.tv_sec) + (double)(f2.tv_nsec - r1.tv_nsec) / (double)1000000000L) * 1000);

  unsigned int num_loops = 1;
  for (unsigned int i = 0; i < num_loops; i++)
    get_recommendations(movies, ratings, uids, (unsigned)mlength, (unsigned)rlength, (unsigned)ulength, flags);

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
