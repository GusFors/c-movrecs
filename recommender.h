#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_THREADS 4

#define DATASET_SMALL 1
#define DATASET_FULL 2
#define DATASET_FULL32 3
#define DATASET_ORIGINAL 4
#define DATASET_EXAMPLE 5
#define DATASET_SMALL_ALT 6

#define NODE_EXP_TEST_SET 1
#define NODE_ORIGINAL_TEST_SET 2
#define C_EXP_TEST_SET 3
#define NODE_ORIGINAL_DIFF_USER_SET 4

// #define DEFAULT_USER 3
// #define DEFAULT_MIN_RATINGS 3

// Set dataset path here or override with -D when compiling
#ifndef DATASET
#  define DATASET DATASET_FULL32
#endif

#ifndef DATA_PATH
#  if DATASET == DATASET_SMALL
#    define DATA_PATH "small"
#    define DEFAULT_USER 3
#    define DEFAULT_MIN_RATINGS 3
#    define MATCHING_TESTS 1
#  elif DATASET == DATASET_FULL
#    define DATA_PATH "full"
#    define DEFAULT_USER 3
#    define DEFAULT_MIN_RATINGS 3
#  elif DATASET == DATASET_FULL32
#    define DATA_PATH "full32"
#    define DEFAULT_USER 3
#    define DEFAULT_MIN_RATINGS 3
#  elif DATASET == DATASET_ORIGINAL
#    define DATA_PATH "original"
#    define DEFAULT_USER 4
#    define DEFAULT_MIN_RATINGS 1
#  elif DATASET == DATASET_EXAMPLE
#    define DATA_PATH "originalexample"
#    define DEFAULT_USER 3
#    define DEFAULT_MIN_RATINGS 1
#  elif DATASET == DATASET_SMALL_ALT
#    define DATA_PATH "small"
#    define DEFAULT_USER 342
#    define DEFAULT_MIN_RATINGS 25
#    define MATCHING_TESTS 1
#  else
#    define DATA_PATH "small"
#  endif
#endif

#ifndef DELIMITER
#  if DATASET == DATASET_ORIGINAL || DATASET == DATASET_EXAMPLE
#    define DELIMITER ";"
#  else
#    define DELIMITER ","
#  endif
#endif

// Set test comparison set here or with -D
#ifndef TEST_SET
#  if DATASET == DATASET_SMALL_ALT
#    define TEST_SET NODE_ORIGINAL_DIFF_USER_SET
#  else
#    define TEST_SET NODE_ORIGINAL_TEST_SET
#  endif
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
  RATINGS_EXTRA_FILTER = 1 << 3,
};

void get_recommendations(unsigned int userid, unsigned int min_numratings, struct movie *movies, struct rating *ratings, unsigned int *uids,
                         unsigned int mlength, unsigned int rlength, unsigned int ulength, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
