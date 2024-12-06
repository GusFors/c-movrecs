#ifndef RATINGS_UTILS_H
#define RATINGS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "recommender.h"

void calc_num_ratings(struct movie *movies, struct rating *ratings, unsigned int mlength, unsigned int rlength);

unsigned int count_user_ratings(unsigned int userid_a, struct rating *filtered_ratings, unsigned int filtered_rlength);

unsigned int filter_movie_numratings(struct movie *movies, unsigned int mlength, unsigned int min_numratings, struct movie_compact *filtered_movies);

unsigned int filter_numratings(struct movie *movies, struct rating *ratings, unsigned int mlength, unsigned int rlength, unsigned int min_numratings,
                               struct rating *filtered_ratings);

#ifdef __cplusplus
}
#endif

#endif
