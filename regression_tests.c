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

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

int test_compare_sim_scores(struct user_sim *calculated_sim_scores) {
  printf(YELLOW_OUTPUT "\ntest compare similarity scores\n" RESET_OUTPUT);

  struct user_sim c_comparisons[] = {
      {1, 0.014652f},   {4, 0.047059f},   {5, 0.047059f},   {6, 0.038835f},   {8, 0.047059f},   {14, 0.137931f},  {15, 0.023529f},  {16, 0.037736f},
      {17, 0.017391f},  {18, 0.023529f},  {19, 0.039216f},  {20, 0.014706f},  {21, 0.028986f},  {22, 0.100000f},  {23, 0.047059f},  {24, 0.075472f},
      {25, 0.047059f},  {27, 0.075472f},  {28, 0.032258f},  {29, 0.029851f},  {31, 0.075472f},  {32, 0.044944f},  {33, 0.018605f},  {34, 0.800000f},
      {38, 0.047059f},  {39, 0.074074f},  {40, 0.075472f},  {41, 0.075472f},  {42, 0.010782f},  {44, 0.200000f},  {45, 0.025974f},  {47, 0.133333f},
      {49, 0.058824f},  {50, 0.037383f},  {51, 0.010554f},  {55, 0.800000f},  {57, 0.014925f},  {58, 0.029851f},  {59, 0.018605f},  {60, 0.047059f},
      {61, 0.058824f},  {62, 0.030303f},  {63, 0.029851f},  {64, 0.009592f},  {65, 0.075472f},  {66, 0.027397f},  {68, 0.012384f},  {70, 0.018605f},
      {71, 0.075472f},  {72, 0.038462f},  {73, 0.307692f},  {74, 0.024096f},  {75, 1.000000f},  {80, 0.075472f},  {82, 0.500000f},  {84, 0.008547f},
      {85, 0.047059f},  {88, 0.064516f},  {89, 0.042553f},  {91, 0.121212f},  {93, 0.022989f},  {95, 0.137931f},  {96, 0.036364f},  {98, 0.137931f},
      {100, 0.075472f}, {101, 0.047059f}, {103, 0.017467f}, {104, 0.012658f}, {105, 0.016807f}, {109, 0.074074f}, {110, 0.039216f}, {111, 0.105263f},
      {113, 0.036364f}, {115, 0.200000f}, {116, 0.081633f}, {117, 0.026490f}, {118, 0.075472f}, {121, 0.137931f}, {122, 0.023529f}, {124, 0.047059f},
      {125, 0.200000f}, {127, 0.307692f}, {129, 0.200000f}, {131, 0.088889f}, {132, 0.035088f}, {133, 0.075472f}, {135, 0.044944f}, {136, 0.137931f},
      {137, 0.022989f}, {138, 0.800000f}, {140, 0.011331f}, {142, 0.047059f}, {144, 0.100000f}, {146, 0.100000f}, {149, 0.047619f}, {152, 0.051948f},
      {153, 0.056338f}, {155, 0.137931f}, {156, 0.050633f}, {157, 0.047059f}, {158, 0.137931f}, {159, 0.061538f}, {160, 0.017544f}, {161, 0.026667f},
      {162, 0.029851f}, {164, 0.307692f}, {166, 0.028169f}, {167, 0.054795f}, {168, 0.029851f}, {169, 0.017391f}, {171, 0.047059f}, {172, 0.043011f},
      {173, 0.047059f}, {174, 0.137931f}, {177, 0.012085f}, {178, 0.058824f}, {180, 0.137931f}, {181, 0.137931f}, {182, 0.009050f}, {183, 0.111111f},
      {185, 0.047059f}, {186, 0.065574f}, {187, 0.027972f}, {188, 0.047059f}, {189, 0.058824f}, {190, 0.047059f}, {191, 0.047059f}, {193, 0.075472f},
      {195, 0.029851f}, {198, 0.025157f}, {199, 0.024096f}, {200, 0.051282f}, {201, 0.039216f}, {202, 0.013423f}, {203, 0.090909f}, {204, 0.026846f},
      {210, 0.800000f}, {211, 0.307692f}, {212, 0.100000f}, {215, 0.030303f}, {216, 0.039216f}, {217, 0.015686f}, {219, 0.044444f}, {221, 0.036364f},
      {222, 0.137931f}, {224, 0.021858f}, {225, 0.137931f}, {226, 0.037383f}, {227, 0.075472f}, {229, 0.075472f}, {230, 0.285714f}, {231, 0.307692f},
      {232, 0.017857f}, {233, 0.034188f}, {234, 0.019417f}, {235, 0.047059f}, {236, 0.137931f}, {237, 0.800000f}, {238, 0.047059f}, {239, 0.058824f},
      {240, 0.051282f}, {241, 0.100000f}, {243, 0.137931f}, {244, 0.117647f}, {246, 0.105263f}, {247, 0.051282f}, {249, 0.032787f}, {251, 0.047059f},
      {254, 0.024096f}, {256, 0.075472f}, {258, 0.075472f}, {260, 0.047059f}, {261, 0.047059f}, {263, 0.016667f}, {264, 0.075472f}, {265, 0.074074f},
      {266, 0.057143f}, {267, 0.036364f}, {268, 0.800000f}, {271, 0.117647f}, {274, 0.012012f}, {275, 0.024096f}, {277, 0.500000f}, {278, 0.029851f},
      {279, 0.500000f}, {280, 0.075472f}, {282, 0.018779f}, {285, 0.200000f}, {286, 0.075472f}, {287, 0.029851f}, {288, 0.008016f}, {290, 0.018265f},
      {292, 0.063492f}, {294, 0.043478f}, {296, 0.058824f}, {297, 0.058824f}, {298, 0.068966f}, {300, 0.047059f}, {301, 0.058824f}, {303, 1.000000f},
      {304, 0.015267f}, {305, 0.011236f}, {307, 0.014815f}, {308, 0.800000f}, {309, 0.027972f}, {312, 0.026490f}, {313, 0.076923f}, {314, 0.020513f},
      {315, 0.137931f}, {317, 0.075472f}, {318, 0.024540f}, {319, 0.047059f}, {320, 0.200000f}, {323, 0.058824f}, {324, 0.137931f}, {325, 0.105263f},
      {326, 0.030303f}, {328, 0.016949f}, {330, 0.022989f}, {332, 0.028169f}, {333, 0.137931f}, {334, 0.075472f}, {335, 0.137931f}, {336, 0.075472f},
      {337, 0.047059f}, {338, 0.047059f}, {339, 0.021622f}, {343, 0.047059f}, {344, 1.000000f}, {345, 0.137931f}, {346, 0.061538f}, {348, 0.047059f},
      {349, 0.075472f}, {350, 0.200000f}, {351, 0.047059f}, {352, 0.044944f}, {354, 0.017857f}, {355, 0.137931f}, {356, 0.017391f}, {357, 0.023529f},
      {359, 0.038462f}, {361, 0.054795f}, {362, 0.033058f}, {366, 0.075472f}, {367, 0.014706f}, {368, 0.014388f}, {369, 0.051282f}, {372, 0.031008f},
      {373, 0.047059f}, {376, 0.051282f}, {377, 0.060606f}, {378, 0.038462f}, {379, 0.137931f}, {380, 0.022472f}, {381, 0.030769f}, {382, 0.019417f},
      {385, 0.036364f}, {387, 0.017857f}, {390, 0.058824f}, {391, 0.015209f}, {395, 0.307692f}, {398, 0.047059f}, {399, 0.075472f}, {400, 0.047059f},
      {402, 0.075472f}, {403, 0.071429f}, {404, 0.075472f}, {405, 0.057971f}, {408, 0.047059f}, {409, 0.137931f}, {410, 0.075472f}, {411, 0.029851f},
      {413, 0.033058f}, {414, 0.006061f}, {416, 0.100000f}, {417, 0.058824f}, {418, 0.058824f}, {419, 0.029851f}, {420, 0.058824f}, {421, 0.026846f},
      {422, 0.047059f}, {424, 0.066667f}, {425, 0.067797f}, {426, 0.029851f}, {427, 0.200000f}, {428, 0.200000f}, {430, 0.500000f}, {432, 0.019048f},
      {433, 0.200000f}, {434, 0.033058f}, {436, 0.024096f}, {437, 0.013841f}, {438, 0.021858f}, {439, 0.058824f}, {440, 0.137931f}, {441, 1.000000f},
      {442, 1.000000f}, {444, 0.047059f}, {445, 0.137931f}, {446, 0.047059f}, {447, 0.075472f}, {448, 0.014706f}, {449, 0.075472f}, {450, 0.075472f},
      {452, 0.040816f}, {453, 0.051282f}, {454, 0.075472f}, {455, 0.047059f}, {457, 1.000000f}, {461, 0.100000f}, {462, 0.055556f}, {463, 0.075472f},
      {464, 0.030075f}, {465, 0.023529f}, {467, 0.047059f}, {469, 0.023121f}, {470, 0.047059f}, {471, 0.058824f}, {474, 0.005634f}, {477, 0.017544f},
      {479, 0.030534f}, {480, 0.010870f}, {482, 0.100000f}, {483, 0.021277f}, {484, 0.014440f}, {488, 0.017391f}, {489, 0.012270f}, {492, 0.044944f},
      {493, 0.307692f}, {495, 0.026490f}, {496, 0.800000f}, {500, 0.051282f}, {501, 0.057971f}, {503, 0.075472f}, {504, 0.137931f}, {509, 0.050633f},
      {510, 0.100000f}, {511, 0.047059f}, {512, 0.047059f}, {514, 0.020725f}, {515, 0.047059f}, {516, 0.137931f}, {517, 0.018519f}, {518, 0.129032f},
      {522, 0.042553f}, {523, 0.030303f}, {524, 0.045977f}, {525, 0.024390f}, {527, 0.059701f}, {532, 0.047059f}, {533, 0.500000f}, {534, 0.022472f},
      {536, 0.047059f}, {537, 0.800000f}, {539, 0.100000f}, {541, 0.047059f}, {542, 0.075472f}, {549, 0.800000f}, {552, 0.044944f}, {553, 0.047059f},
      {554, 0.015152f}, {555, 0.011236f}, {557, 0.075472f}, {560, 0.044944f}, {561, 0.017621f}, {562, 0.011905f}, {563, 0.075472f}, {565, 0.047059f},
      {566, 0.047059f}, {570, 0.058824f}, {571, 0.057971f}, {572, 0.016667f}, {576, 0.051282f}, {577, 0.018519f}, {579, 0.036364f}, {580, 0.029197f},
      {581, 0.026846f}, {586, 0.200000f}, {587, 0.028986f}, {588, 0.075472f}, {589, 0.047059f}, {590, 0.010363f}, {593, 0.043011f}, {594, 0.047059f},
      {596, 0.036697f}, {597, 0.011869f}, {599, 0.009780f}, {600, 0.014760f}, {601, 0.029851f}, {602, 0.036364f}, {603, 0.015209f}, {605, 0.800000f},
      {606, 0.010390f}, {607, 0.026490f}, {608, 0.015504f}, {610, 0.032787f},
  };

  unsigned int num_exact_matchings_ids_c = 0;
  unsigned int num_matchings_scores_c = 0;
  unsigned int num_warnings = 0;

  for (size_t i = 0; i < ARRAY_LEN(c_comparisons); i++) {
    if (c_comparisons[i].user_id == calculated_sim_scores[i].user_id) {
      num_exact_matchings_ids_c++;
      continue;
    }

    printf("" FAIL_RED " simscore userid does not match: expected userid: %d, actual: %d\n", c_comparisons[i].user_id,
           calculated_sim_scores[i].user_id);
  }

  for (size_t i = 0; i < ARRAY_LEN(c_comparisons); i++) {
    double score_diff = fabs(c_comparisons[i].simscore - calculated_sim_scores[i].simscore);

    if (score_diff > 0.005) {
      printf("" FAIL_RED " simscore diff for userid: %d, expected simscore: %f, actual: %f\n", c_comparisons[i].user_id, c_comparisons[i].simscore,
             calculated_sim_scores[i].simscore);
      continue;
    }

    if (score_diff < 0.0001) {
      num_matchings_scores_c++;
      continue;
    }

    if (score_diff > 0.001) {
      printf("" WARNING_YELLOW " small simscore diff for userid: %d, expected simscore: %f, actual: %f\n", c_comparisons[i].user_id,
             c_comparisons[i].simscore, calculated_sim_scores[i].simscore);
      num_warnings++;
      num_matchings_scores_c++;
      continue;
    }
  }

  printf("num_exact_matchings_uids: %d/%lu \n", num_exact_matchings_ids_c, ARRAY_LEN(c_comparisons));
  printf("num matchings_simscores:  %d/%lu \n", num_matchings_scores_c, ARRAY_LEN(c_comparisons));

  // printf("" WARNING_YELLOW " low amount of exactly matching uids\n");
  if (num_exact_matchings_ids_c < ARRAY_LEN(c_comparisons) || num_matchings_scores_c < ARRAY_LEN(c_comparisons)) {
    printf("similarity score test: " FAIL_RED);
    return RETURN_FAIL;
  }

  printf("similarity score test: ");
  num_warnings ? printf(PASS_GREEN " - %d warnings", num_warnings) : printf(PASS_GREEN);
  return RETURN_SUCCESS;
}

int test_compare_movie_ids(struct movie_recommendation *calculated_recommendations) {
  printf(YELLOW_OUTPUT "\ntest compare movie ids\n" RESET_OUTPUT);

  unsigned int node_test_movids_exp_version[100] = {
      26810,  2239,   6460, 7121,  334,   174053, 1964,  177593, 4441,   7008,   2511,  4956,  2925,   6051,   9010,  31364, 2351,
      4444,   3224,   3384, 5833,  213,   168418, 30803, 57274,  121231, 87234,  7842,  27801, 55363,  85736,  90439, 3727,  898,
      115122, 98154,  6345, 27397, 80139, 1836,   51931, 117192, 26680,  104879, 58301, 2436,  106642, 103688, 59141, 942,   3152,
      123,    112804, 3972, 4765,  3771,  26171,  2131,  112175, 1046,   55167,  2349,  47423, 3451,   4334,   1303,  58295, 80906,
      1733,   1238,   1104, 893,   1611,  87529,  3342,  171763, 187595, 80831,  3201,  2035,  42632,  1217,   1273,  3275,  3681,
      89904,  1051,   5466, 1178,  6618,  27722,  26082, 27831,  6306,   162,    6235,  1041,  5404,   3111,   2360};

  unsigned int node_test_movids_old_version[100] = {
      26810, 2239,   6460,  7121,  334,   174053, 1964,  177593, 4441,   7008,   2511,   4956,  2925,   6051,   9010,  2351,  31364,
      4444,  3224,   3384,  57274, 213,   168418, 30803, 5833,   115122, 87234,  27801,  7842,  3727,   121231, 55363, 58301, 85736,
      90439, 898,    98154, 6345,  80139, 1836,   51931, 104879, 117192, 26680,  2436,   27397, 103688, 106642, 59141, 942,   3152,
      123,   112804, 3972,  4765,  3771,  26171,  2131,  1046,   112175, 55167,  2349,   47423, 3451,   4334,   58295, 1303,  87529,
      80906, 42632,  1733,  1238,  1104,  893,    1611,  80831,  3342,   171763, 187595, 3201,  2035,   1217,   3275,  1273,  3681,
      89904, 1051,   5466,  1178,  6618,  27831,  26082, 8042,   6306,   162,    6235,   1041,  5404,   3111,   2360,
  };

  unsigned int cver_test_movids_exp[100] = {
      26810, 2239, 6460,  7121,  334,   174053, 1964,   177593, 4441,   7008,  2511,   4956,   2925,   6051,   9010,  2351,  31364,
      4444,  3224, 3384,  57274, 213,   168418, 30803,  5833,   115122, 87234, 27801,  7842,   3727,   121231, 55363, 58301, 85736,
      90439, 898,  6345,  80139, 1836,  51931,  104879, 117192, 26680,  98154, 2436,   27397,  103688, 106642, 59141, 942,   3152,
      123,   3972, 4765,  3771,  26171, 2131,   1046,   112175, 55167,  2349,  47423,  3451,   112804, 4334,   58295, 1303,  80906,
      87529, 2035, 42632, 1733,  1238,  1104,   893,    1611,   80831,  3342,  171763, 187595, 3201,   1217,   3275,  1273,  3681,
      1051,  5466, 1178,  89904, 6618,  27831,  26082,  8042,   6306,   162,   1041,   6235,   5404,   3111,   2360,
  };

  unsigned int num_exact_matchings_ids_node = 0;
  unsigned int num_exact_matchings_ids_node_old = 0;
  unsigned int num_exact_matchings_ids_c = 0;

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

int test_check_duplicated_movie_ids(struct movie_recommendation *calculated_recommendations, unsigned int num_recs) {
  int pass = 1;
  printf(YELLOW_OUTPUT "\ntest duplicate movids, " RESET_OUTPUT "total recs: %d\n", num_recs);

  unsigned int highest_id = 0;
  for (unsigned int i = 0; i < num_recs; i++) {
    if (calculated_recommendations[i].movie_id > highest_id)
      highest_id = calculated_recommendations[i].movie_id;
  }

  printf("highest movid: %d\n", highest_id);

  int mov_id_count[highest_id + 1];

  for (unsigned int i = 0; i <= highest_id; i++) {
    mov_id_count[i] = 0;
  }

  for (unsigned int i = 0; i < num_recs; i++) {
    mov_id_count[calculated_recommendations[i].movie_id] += 1;
  }

  unsigned int count = 0;
  for (unsigned int i = 0; i <= highest_id; i++) {
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

int test_compare_scores_diff(struct movie_recommendation *calculated_recommendations, unsigned int num_recs) {
  double max_avg_diff = 0.05;
  double max_single_diff = 0.02;
  printf(YELLOW_OUTPUT "\ntest score difference\n" RESET_OUTPUT);

  struct movie_recommendation c_comparisons[] = {
      [0] = {.movie_id = 26810, .num_ratings = 3, .recommendation_score = 4.9691f},
      [1] = {.movie_id = 2239, .num_ratings = 6, .recommendation_score = 4.9557f},
      [2] = {.movie_id = 98154, .num_ratings = 4, .recommendation_score = 4.7313f},
      [3] = {.movie_id = 100507, .num_ratings = 4, .recommendation_score = 3.9442f},
      [4] = {.movie_id = 307, .num_ratings = 24, .recommendation_score = 3.9752f},
      [5] = {.movie_id = 68954, .num_ratings = 105, .recommendation_score = 3.9698f},
  };

  struct movie_recommendation node_comparisons_old[] = {
      [0] = {.movie_id = 26810, .num_ratings = 3, .recommendation_score = 4.9691f},
      [1] = {.movie_id = 2239, .num_ratings = 6, .recommendation_score = 4.9557f},
      [2] = {.movie_id = 98154, .num_ratings = 4, .recommendation_score = 4.7583f},
      [3] = {.movie_id = 100507, .num_ratings = 4, .recommendation_score = 3.9442f},
      [4] = {.movie_id = 307, .num_ratings = 24, .recommendation_score = 3.9752f},
      [5] = {.movie_id = 68954, .num_ratings = 105, .recommendation_score = 3.9701f},
  };

  printf("c comparison\n");
  double avg_diff = 0;
  double max_diff = 0;
  int matches = 0;
  for (unsigned int i = 0; i < 5; i++) {
    for (unsigned int y = 0; y < num_recs; y++) {
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

  for (unsigned int i = 0; i < 5; i++) {
    for (unsigned int y = 0; y < num_recs; y++) {
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
