#include "recommender.h"
#include <stdio.h>
#include <math.h>

#define GREEN_OUTPUT "\x1b[32m"
#define RED_OUTPUT "\x1b[31m"
#define YELLOW_OUTPUT "\x1b[33m"
#define RESET_OUTPUT "\x1b[0m"

#define PASS_GREEN "" GREEN_OUTPUT "PASS" RESET_OUTPUT ""
#define FAIL_RED "" RED_OUTPUT "FAIL" RESET_OUTPUT ""
#define WARNING_YELLOW "" YELLOW_OUTPUT "WARN" RESET_OUTPUT ""

#define RETURN_FAIL 0
#define RETURN_SUCCESS 1

int test_compare_movie_ids(struct movie_recommendation *calculated_recommendations) {
  printf("\ntest compare movie ids\n");

  int node_test_movids_exp_version[100] = {
      26810, 2239,   6460,   7121,   334,   174053, 1964,   177593, 4441,  7008,  2511,  4956,  2925,   6051,  9010,   31364,  2351,   4444,  3224,   3384,
      5833,  213,    168418, 30803,  57274, 121231, 87234,  7842,   27801, 55363, 85736, 90439, 3727,   898,   115122, 98154,  6345,   27397, 80139,  1836,
      51931, 117192, 26680,  104879, 58301, 2436,   106642, 103688, 59141, 942,   3152,  123,   112804, 3972,  4765,   3771,   26171,  2131,  112175, 1046,
      55167, 2349,   47423,  3451,   4334,  1303,   58295,  80906,  1733,  1238,  1104,  893,   1611,   87529, 3342,   171763, 187595, 80831, 3201,   2035,
      42632, 1217,   1273,   3275,   3681,  89904,  1051,   5466,   1178,  6618,  27722, 26082, 27831,  6306,  162,    6235,   1041,   5404,  3111,   2360};

  int node_test_movids_old_version[100] = {
      26810, 2239,   6460,   7121,  334,  174053, 1964,   177593, 4441,  7008,  2511,   4956,  2925,   6051,  9010,  2351,  31364, 4444,   3224,   3384,
      57274, 213,    168418, 30803, 5833, 115122, 87234,  27801,  7842,  3727,  121231, 55363, 58301,  85736, 90439, 898,   98154, 6345,   80139,  1836,
      51931, 104879, 117192, 26680, 2436, 27397,  103688, 106642, 59141, 942,   3152,   123,   112804, 3972,  4765,  3771,  26171, 2131,   1046,   112175,
      55167, 2349,   47423,  3451,  4334, 58295,  1303,   87529,  80906, 42632, 1733,   1238,  1104,   893,   1611,  80831, 3342,  171763, 187595, 3201,
      2035,  1217,   3275,   1273,  3681, 89904,  1051,   5466,   1178,  6618,  27831,  26082, 8042,   6306,  162,   6235,  1041,  5404,   3111,   2360,
  };

  int cver_test_movids_exp[100] = {
      26810,  2239,   6460,   7121,   334,  174053, 1964,   177593, 4441,  7008, 2511,   4956,  2925,  6051,  9010,  2351,  31364, 4444,  3224,   3384,
      57274,  213,    168418, 30803,  5833, 115122, 87234,  27801,  7842,  3727, 121231, 55363, 58301, 85736, 90439, 898,   6345,  80139, 1836,   51931,
      104879, 117192, 26680,  98154,  2436, 27397,  103688, 106642, 59141, 942,  3152,   123,   3972,  4765,  3771,  26171, 2131,  1046,  112175, 55167,
      2349,   47423,  3451,   112804, 4334, 58295,  1303,   80906,  87529, 2035, 42632,  1733,  1238,  1104,  893,   1611,  80831, 3342,  171763, 187595,
      3201,   1217,   3275,   1273,   3681, 1051,   5466,   1178,   89904, 6618, 27831,  26082, 8042,  6306,  162,   1041,  6235,  5404,  3111,   2360,
  };

  int num_exact_matchings_ids_node = 0;
  int num_exact_matchings_ids_node_old = 0;
  int num_exact_matchings_ids_c = 0;

  for (int i = 0; i < 100; i++) {
    if (node_test_movids_exp_version[i] == calculated_recommendations[i].movie_id)
      num_exact_matchings_ids_node++;
  }

  for (int i = 0; i < 100; i++) {
    if (node_test_movids_old_version[i] == calculated_recommendations[i].movie_id)
      num_exact_matchings_ids_node_old++;
  }

  for (int i = 0; i < 100; i++) {
    if (cver_test_movids_exp[i] == calculated_recommendations[i].movie_id)
      num_exact_matchings_ids_c++;
  }

  printf("num_exact_matchings_ids_n_exp:" GREEN_OUTPUT " %d" RESET_OUTPUT "/100 \n", num_exact_matchings_ids_node);
  printf("num_exact_matchings_ids_n_old: %d/100\n", num_exact_matchings_ids_node_old);
  printf("num_exact_matchings_ids_c_exp: %d/100\n", num_exact_matchings_ids_c);

  if (num_exact_matchings_ids_c < 30 || num_exact_matchings_ids_node < 30 || num_exact_matchings_ids_node_old < 30)
    printf("" WARNING_YELLOW " low amount of exactly matching movids\n");

  return 1;
}

int test_check_duplicated_movie_ids(struct movie_recommendation *calculated_recommendations, int num_recs) {
  int pass = 1;
  printf("\ntest duplicate movids, total recs: %d\n", num_recs);

  int highest_id = 0;
  for (int i = 0; i < num_recs; i++) {
    if (calculated_recommendations[i].movie_id > highest_id)
      highest_id = calculated_recommendations[i].movie_id;
  }

  printf("highest movid: %d\n", highest_id);

  int mov_id_count[highest_id + 1];

  for (int i = 0; i <= highest_id; i++) {
    mov_id_count[i] = 0;
  }

  for (int i = 0; i < num_recs; i++) {
    mov_id_count[calculated_recommendations[i].movie_id] += 1;
  }

  int count = 0;
  for (int i = 0; i <= highest_id; i++) {
    if (mov_id_count[i] > 1) {
      printf("" RED_OUTPUT "duplicate movid at index: %d, count: %d " RESET_OUTPUT "\n", i, mov_id_count[i]);
      pass = 0;
      count++;
    }
  }

  printf("movid duplicates: %d ", count);
  pass ? printf(PASS_GREEN) : printf(FAIL_RED);
  printf("\n");

  return 1;
}

int test_compare_scores_diff(struct movie_recommendation *calculated_recommendations, int num_recs) {

  int pass = 1;
  double max_avg_diff = 0.05;
  double max_single_diff = 0.02;
  printf("\ntest score difference\n");

  struct movie_recommendation c_comparisons[] = {
      [0] = {.movie_id = 26810, .num_ratings = 3, .recommendation_score = 4.9691},
      [1] = {.movie_id = 2239, .num_ratings = 6, .recommendation_score = 4.9557},
      [2] = {.movie_id = 98154, .num_ratings = 4, .recommendation_score = 4.7313},
      [3] = {.movie_id = 100507, .num_ratings = 4, .recommendation_score = 3.9442},
      [4] = {.movie_id = 307, .num_ratings = 24, .recommendation_score = 3.9752},
      [5] = {.movie_id = 68954, .num_ratings = 105, .recommendation_score = 3.9698},
  };

  struct movie_recommendation node_comparisons_old[] = {
      [0] = {.movie_id = 26810, .num_ratings = 3, .recommendation_score = 4.9691},
      [1] = {.movie_id = 2239, .num_ratings = 6, .recommendation_score = 4.9557},
      [2] = {.movie_id = 98154, .num_ratings = 4, .recommendation_score = 4.7583},
      [3] = {.movie_id = 100507, .num_ratings = 4, .recommendation_score = 3.9442},
      [4] = {.movie_id = 307, .num_ratings = 24, .recommendation_score = 3.9752},
      [5] = {.movie_id = 68954, .num_ratings = 105, .recommendation_score = 3.9701},
  };

  printf("c comparison\n");
  double avg_diff = 0;
  double max_diff = 0;
  int matches = 0;
  for (int i = 0; i < 5; i++) {
    for (int y = 0; y < num_recs; y++) {
      if (c_comparisons[i].movie_id == calculated_recommendations[y].movie_id) {
        matches++;

        double score_diff = fabs(c_comparisons[i].recommendation_score - calculated_recommendations[y].recommendation_score);
        avg_diff += score_diff;

        if (score_diff > max_diff)
          max_diff = score_diff;
      }
    }
  }

  if (matches == 0) {
    printf("" RED_OUTPUT "could not match any movids for comparing score difference " RESET_OUTPUT "\n");
    return RETURN_FAIL;
  }

  double average = avg_diff / matches;
  printf("c average score diff: %f ", average);
  average < max_avg_diff ? printf(PASS_GREEN) : printf(FAIL_RED);
  printf(" total matched and compared:%d", matches);

  printf("\nc max single score diff: %f ", max_diff);
  max_diff < max_single_diff ? printf(PASS_GREEN) : printf(FAIL_RED);

  printf("\nnode comparison\n");
  avg_diff = 0;
  max_diff = 0;
  matches = 0;
  for (int i = 0; i < 5; i++) {
    for (int y = 0; y < num_recs; y++) {
      if (c_comparisons[i].movie_id == calculated_recommendations[y].movie_id) {
        matches++;

        double score_diff = fabs(node_comparisons_old[i].recommendation_score - calculated_recommendations[y].recommendation_score);
        avg_diff += score_diff;

        if (score_diff > max_diff)
          max_diff = score_diff;
      }
    }
  }

  average = avg_diff / matches;
  printf("node average score diff: %f ", average);
  average < max_avg_diff ? printf(PASS_GREEN) : printf(FAIL_RED);

  printf("\nnode max single score diff: %f ", max_diff);
  max_diff < max_single_diff ? printf(PASS_GREEN) : printf(FAIL_RED);

  printf("\n");

  return 1;
}
