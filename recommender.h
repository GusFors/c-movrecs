#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_THREADS 4

#define DATASET_SMALL 1
#define DATASET_FULL 2
#define DATASET_FULL32 3

#define NODE_EXP_TEST_SET 1
#define NODE_ORIGINAL_TEST_SET 2
#define C_EXP_TEST_SET 3

// Set dataset path here or override with -D when compiling
#ifndef DATASET
#  define DATASET DATASET_SMALL
#endif

#ifndef DATA_PATH
#  if DATASET == DATASET_SMALL
#    define DATA_PATH "small"
#  elif DATASET == DATASET_FULL
#    define DATA_PATH "full"
#  elif DATASET == DATASET_FULL32
#    define DATA_PATH "full32"
#  else
#    define DATA_PATH "small"
#  endif
#endif

// Set test comparison set here or with -D
#ifndef TEST_SET
#  define TEST_SET NODE_ORIGINAL_TEST_SET
#endif

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
  VERBOSE_DEBUG = 1 << 1,
  SINGLE_THREADED = 1 << 2,
};

void get_recommendations(struct movie *movies, struct rating *ratings, unsigned int *uids, unsigned int mlength, unsigned int rlength,
                         unsigned int ulength, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
