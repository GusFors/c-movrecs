#ifndef TESTS_H
#define TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define GREEN_OUTPUT "\x1b[32m"
#define RED_OUTPUT "\x1b[31m"
#define YELLOW_OUTPUT "\x1b[33m"
#define RESET_OUTPUT "\x1b[0m"

#define PASS_GREEN "" GREEN_OUTPUT "PASS" RESET_OUTPUT ""
#define FAIL_RED "" RED_OUTPUT "FAIL" RESET_OUTPUT ""
#define WARNING_YELLOW "" YELLOW_OUTPUT "WARN" RESET_OUTPUT ""

#define RETURN_FAIL 0
#define RETURN_SUCCESS 1

int test_compare_movie_ids(struct movie_recommendation *calculated_recommendations);
int test_check_duplicated_movie_ids(struct movie_recommendation *calculated_recommendations, int num_recs);
int test_compare_scores_diff(struct movie_recommendation *calculated_recommendations, int num_recs);

#ifdef __cplusplus
}
#endif

#endif