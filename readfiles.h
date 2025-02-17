#ifndef READFILES_H
#define READFILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "recommender.h"

ssize_t read_ratings(struct rating *ratings_p);

ssize_t read_ratings_fast(struct rating *ratings_p);

ssize_t read_ratings_fast_gl(struct rating *ratings_p);

ssize_t read_ratings_fast_mem_gl(struct rating *ratings_p);

ssize_t read_ratings_fast_mmap(struct rating *ratings_p);

ssize_t read_ratings_lines(void);

ssize_t read_movies(struct movie *movies_p, struct movie_title *mov_titles);

ssize_t read_movies_lines(void);

size_t read_users_from_ratings(unsigned int *uids, struct rating *ratings_p, unsigned int rlength);

size_t read_users_num(struct rating *ratings_p, unsigned int rlength);

#ifdef __cplusplus
}
#endif

#endif
