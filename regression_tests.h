#ifndef TESTS_H
#define TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define GREEN_OUTPUT "\x1b[32m"
#define RED_OUTPUT "\x1b[31m"
#define YELLOW_OUTPUT "\x1b[33m"
#define RESET_OUTPUT "\x1b[0m"

#define PASS_GREEN "" GREEN_OUTPUT "PASS" RESET_OUTPUT ""
#define FAIL_RED "" RED_OUTPUT "FAIL" RESET_OUTPUT ""
#define WARNING_YELLOW "" YELLOW_OUTPUT "WARN" RESET_OUTPUT ""

#define RETURN_FAIL 0
#define RETURN_SUCCESS 1

struct test_comparison_data {
  char *title;
  struct user_sim *similarity_scores;
  struct movie_recommendation *movie_recommendations;
  size_t simscores_len;
  size_t movrec_len;
};

int test_compare_sim_scores(struct user_sim *calculated_sim_scores, unsigned int calculated_sim_scores_length, unsigned int user_id);
int test_compare_sim_scores_n(struct user_sim *calculated_sim_scores, struct test_comparison_data *comparison_scores);
int test_movie_recommendation_tests(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length, unsigned int user_id);
int test_compare_movrec_scores(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length);
int test_compare_movrec_numratings(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length);
int test_compare_movrec_movids(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length);
int test_check_duplicated_movie_ids(struct movie_recommendation *calculated_recommendations, unsigned int num_recs);
int test_compare_scores_diff(struct movie_recommendation *calculated_recommendations, unsigned int num_recs);
void node_old_comparisons(struct test_comparison_data *t);

#ifdef __cplusplus
}
#endif

#endif
