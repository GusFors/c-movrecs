#ifndef READFILES_H
#define READFILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "recommender.h"

ssize_t read_ratings(struct rating *ratings_p);

ssize_t read_ratings_fast(struct rating *ratings_p);

ssize_t read_ratings_lines();

ssize_t read_movies(struct movie *movies_p, struct movie_title *mov_titles);

ssize_t read_movies_lines();

size_t read_users_from_ratings(unsigned int *uids, struct rating *ratings_p, unsigned int rlength);

size_t read_users_num(struct rating *ratings_p, unsigned int rlength);

// int read_users(unsigned int *users, unsigned int *uids);

#ifdef __cplusplus
}
#endif

#endif