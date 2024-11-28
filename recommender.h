#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_THREADS 4

struct rating {
  unsigned int user_id;
  unsigned int movie_id;
  float rating;
};

struct movie {
  unsigned int movie_id;
  unsigned int num_ratings;
  // char title[256];
  // char title[128];
};

struct movie_compact {
  unsigned int movie_id;
  unsigned int num_ratings;
};

struct user {
  unsigned int user_id;
  char name[64];
};

struct user_sim {
  unsigned int user_id;
  float simscore;
};

struct weighted_score {
  unsigned int movie_id;
  float weighted_rating;
  float simscore;
};

struct movie_recommendation {
  unsigned int movie_id;
  unsigned int num_ratings;
  float recommendation_score;
};

struct movie_title {
  char title[256];
  // id
};

enum flags {
  TESTS = 1 << 0,
  SINGLE_THREADED = 1 << 1,
};

void get_recommendations(struct movie *movies, struct rating *ratings, unsigned int *uids, unsigned int mlength, unsigned int rlength,
                         unsigned int ulength, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif