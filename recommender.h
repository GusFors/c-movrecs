#ifndef RECOMMENDER_H
#define RECOMMENDER_H

struct rating {
  unsigned int user_id;
  unsigned int movie_id;
  float rating;
};

struct movie {
  unsigned int movie_id;
  unsigned int num_ratings;
  char title[128];
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif