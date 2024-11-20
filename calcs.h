#ifndef CALCS_H
#define CALCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "recommender.h"

void calc_num_ratings(struct movie *movies, struct rating *ratings, int mlength, int rlength);

int filter_numratings(struct movie *movies, struct rating *ratings, int mlength, int rlength, int min_numratings, struct rating *filtered_ratings);

int filter_movie_numratings(struct movie *movies, int mlength, int min_numratings, struct movie_compact *filtered_movies);

#ifdef __cplusplus
}
#endif

#endif