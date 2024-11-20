#ifndef READFILES_H
#define READFILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "recommender.h"

unsigned int read_ratings(struct rating *ratings_p);

unsigned int read_ratings_fast(struct rating *ratings_p);

unsigned int read_ratings_lines();

unsigned int read_movies(struct movie *movies_p);

unsigned int read_movies_lines();

unsigned int read_users(unsigned int *users, unsigned int *uids);

unsigned int read_users_from_ratings(unsigned int *uids, struct rating *ratings_p, unsigned int rlength);

unsigned int read_users_num(struct rating *ratings_p, unsigned int rlength);

#ifdef __cplusplus
}
#endif

#endif