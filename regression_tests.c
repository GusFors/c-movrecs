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
      {1, 0.014652},   {4, 0.047059},   {5, 0.047059},   {6, 0.038835},   {8, 0.047059},   {14, 0.137931},  {15, 0.023529},  {16, 0.037736},
      {17, 0.017391},  {18, 0.023529},  {19, 0.039216},  {20, 0.014706},  {21, 0.028986},  {22, 0.100000},  {23, 0.047059},  {24, 0.075472},
      {25, 0.047059},  {27, 0.075472},  {28, 0.032258},  {29, 0.029851},  {31, 0.075472},  {32, 0.044944},  {33, 0.018605},  {34, 0.800000},
      {38, 0.047059},  {39, 0.074074},  {40, 0.075472},  {41, 0.075472},  {42, 0.010782},  {44, 0.200000},  {45, 0.025974},  {47, 0.133333},
      {49, 0.058824},  {50, 0.037383},  {51, 0.010554},  {55, 0.800000},  {57, 0.014925},  {58, 0.029851},  {59, 0.018605},  {60, 0.047059},
      {61, 0.058824},  {62, 0.030303},  {63, 0.029851},  {64, 0.009592},  {65, 0.075472},  {66, 0.027397},  {68, 0.012384},  {70, 0.018605},
      {71, 0.075472},  {72, 0.038462},  {73, 0.307692},  {74, 0.024096},  {75, 1.000000},  {80, 0.075472},  {82, 0.500000},  {84, 0.008547},
      {85, 0.047059},  {88, 0.064516},  {89, 0.042553},  {91, 0.121212},  {93, 0.022989},  {95, 0.137931},  {96, 0.036364},  {98, 0.137931},
      {100, 0.075472}, {101, 0.047059}, {103, 0.017467}, {104, 0.012658}, {105, 0.016807}, {109, 0.074074}, {110, 0.039216}, {111, 0.105263},
      {113, 0.036364}, {115, 0.200000}, {116, 0.081633}, {117, 0.026490}, {118, 0.075472}, {121, 0.137931}, {122, 0.023529}, {124, 0.047059},
      {125, 0.200000}, {127, 0.307692}, {129, 0.200000}, {131, 0.088889}, {132, 0.035088}, {133, 0.075472}, {135, 0.044944}, {136, 0.137931},
      {137, 0.022989}, {138, 0.800000}, {140, 0.011331}, {142, 0.047059}, {144, 0.100000}, {146, 0.100000}, {149, 0.047619}, {152, 0.051948},
      {153, 0.056338}, {155, 0.137931}, {156, 0.050633}, {157, 0.047059}, {158, 0.137931}, {159, 0.061538}, {160, 0.017544}, {161, 0.026667},
      {162, 0.029851}, {164, 0.307692}, {166, 0.028169}, {167, 0.054795}, {168, 0.029851}, {169, 0.017391}, {171, 0.047059}, {172, 0.043011},
      {173, 0.047059}, {174, 0.137931}, {177, 0.012085}, {178, 0.058824}, {180, 0.137931}, {181, 0.137931}, {182, 0.009050}, {183, 0.111111},
      {185, 0.047059}, {186, 0.065574}, {187, 0.027972}, {188, 0.047059}, {189, 0.058824}, {190, 0.047059}, {191, 0.047059}, {193, 0.075472},
      {195, 0.029851}, {198, 0.025157}, {199, 0.024096}, {200, 0.051282}, {201, 0.039216}, {202, 0.013423}, {203, 0.090909}, {204, 0.026846},
      {210, 0.800000}, {211, 0.307692}, {212, 0.100000}, {215, 0.030303}, {216, 0.039216}, {217, 0.015686}, {219, 0.044444}, {221, 0.036364},
      {222, 0.137931}, {224, 0.021858}, {225, 0.137931}, {226, 0.037383}, {227, 0.075472}, {229, 0.075472}, {230, 0.285714}, {231, 0.307692},
      {232, 0.017857}, {233, 0.034188}, {234, 0.019417}, {235, 0.047059}, {236, 0.137931}, {237, 0.800000}, {238, 0.047059}, {239, 0.058824},
      {240, 0.051282}, {241, 0.100000}, {243, 0.137931}, {244, 0.117647}, {246, 0.105263}, {247, 0.051282}, {249, 0.032787}, {251, 0.047059},
      {254, 0.024096}, {256, 0.075472}, {258, 0.075472}, {260, 0.047059}, {261, 0.047059}, {263, 0.016667}, {264, 0.075472}, {265, 0.074074},
      {266, 0.057143}, {267, 0.036364}, {268, 0.800000}, {271, 0.117647}, {274, 0.012012}, {275, 0.024096}, {277, 0.500000}, {278, 0.029851},
      {279, 0.500000}, {280, 0.075472}, {282, 0.018779}, {285, 0.200000}, {286, 0.075472}, {287, 0.029851}, {288, 0.008016}, {290, 0.018265},
      {292, 0.063492}, {294, 0.043478}, {296, 0.058824}, {297, 0.058824}, {298, 0.068966}, {300, 0.047059}, {301, 0.058824}, {303, 1.000000},
      {304, 0.015267}, {305, 0.011236}, {307, 0.014815}, {308, 0.800000}, {309, 0.027972}, {312, 0.026490}, {313, 0.076923}, {314, 0.020513},
      {315, 0.137931}, {317, 0.075472}, {318, 0.024540}, {319, 0.047059}, {320, 0.200000}, {323, 0.058824}, {324, 0.137931}, {325, 0.105263},
      {326, 0.030303}, {328, 0.016949}, {330, 0.022989}, {332, 0.028169}, {333, 0.137931}, {334, 0.075472}, {335, 0.137931}, {336, 0.075472},
      {337, 0.047059}, {338, 0.047059}, {339, 0.021622}, {343, 0.047059}, {344, 1.000000}, {345, 0.137931}, {346, 0.061538}, {348, 0.047059},
      {349, 0.075472}, {350, 0.200000}, {351, 0.047059}, {352, 0.044944}, {354, 0.017857}, {355, 0.137931}, {356, 0.017391}, {357, 0.023529},
      {359, 0.038462}, {361, 0.054795}, {362, 0.033058}, {366, 0.075472}, {367, 0.014706}, {368, 0.014388}, {369, 0.051282}, {372, 0.031008},
      {373, 0.047059}, {376, 0.051282}, {377, 0.060606}, {378, 0.038462}, {379, 0.137931}, {380, 0.022472}, {381, 0.030769}, {382, 0.019417},
      {385, 0.036364}, {387, 0.017857}, {390, 0.058824}, {391, 0.015209}, {395, 0.307692}, {398, 0.047059}, {399, 0.075472}, {400, 0.047059},
      {402, 0.075472}, {403, 0.071429}, {404, 0.075472}, {405, 0.057971}, {408, 0.047059}, {409, 0.137931}, {410, 0.075472}, {411, 0.029851},
      {413, 0.033058}, {414, 0.006061}, {416, 0.100000}, {417, 0.058824}, {418, 0.058824}, {419, 0.029851}, {420, 0.058824}, {421, 0.026846},
      {422, 0.047059}, {424, 0.066667}, {425, 0.067797}, {426, 0.029851}, {427, 0.200000}, {428, 0.200000}, {430, 0.500000}, {432, 0.019048},
      {433, 0.200000}, {434, 0.033058}, {436, 0.024096}, {437, 0.013841}, {438, 0.021858}, {439, 0.058824}, {440, 0.137931}, {441, 1.000000},
      {442, 1.000000}, {444, 0.047059}, {445, 0.137931}, {446, 0.047059}, {447, 0.075472}, {448, 0.014706}, {449, 0.075472}, {450, 0.075472},
      {452, 0.040816}, {453, 0.051282}, {454, 0.075472}, {455, 0.047059}, {457, 1.000000}, {461, 0.100000}, {462, 0.055556}, {463, 0.075472},
      {464, 0.030075}, {465, 0.023529}, {467, 0.047059}, {469, 0.023121}, {470, 0.047059}, {471, 0.058824}, {474, 0.005634}, {477, 0.017544},
      {479, 0.030534}, {480, 0.010870}, {482, 0.100000}, {483, 0.021277}, {484, 0.014440}, {488, 0.017391}, {489, 0.012270}, {492, 0.044944},
      {493, 0.307692}, {495, 0.026490}, {496, 0.800000}, {500, 0.051282}, {501, 0.057971}, {503, 0.075472}, {504, 0.137931}, {509, 0.050633},
      {510, 0.100000}, {511, 0.047059}, {512, 0.047059}, {514, 0.020725}, {515, 0.047059}, {516, 0.137931}, {517, 0.018519}, {518, 0.129032},
      {522, 0.042553}, {523, 0.030303}, {524, 0.045977}, {525, 0.024390}, {527, 0.059701}, {532, 0.047059}, {533, 0.500000}, {534, 0.022472},
      {536, 0.047059}, {537, 0.800000}, {539, 0.100000}, {541, 0.047059}, {542, 0.075472}, {549, 0.800000}, {552, 0.044944}, {553, 0.047059},
      {554, 0.015152}, {555, 0.011236}, {557, 0.075472}, {560, 0.044944}, {561, 0.017621}, {562, 0.011905}, {563, 0.075472}, {565, 0.047059},
      {566, 0.047059}, {570, 0.058824}, {571, 0.057971}, {572, 0.016667}, {576, 0.051282}, {577, 0.018519}, {579, 0.036364}, {580, 0.029197},
      {581, 0.026846}, {586, 0.200000}, {587, 0.028986}, {588, 0.075472}, {589, 0.047059}, {590, 0.010363}, {593, 0.043011}, {594, 0.047059},
      {596, 0.036697}, {597, 0.011869}, {599, 0.009780}, {600, 0.014760}, {601, 0.029851}, {602, 0.036364}, {603, 0.015209}, {605, 0.800000},
      {606, 0.010390}, {607, 0.026490}, {608, 0.015504}, {610, 0.032787},
  };

  int num_exact_matchings_ids_c = 0;
  int num_matchings_scores_c = 0;
  int num_warnings = 0;

  for (int i = 0; i < ARRAY_LEN(c_comparisons); i++) {
    if (c_comparisons[i].user_id == calculated_sim_scores[i].user_id) {
      num_exact_matchings_ids_c++;
      continue;
    }

    printf("" FAIL_RED " simscore userid does not match: expected userid: %d, actual: %d\n", c_comparisons[i].user_id,
           calculated_sim_scores[i].user_id);
  }

  for (int i = 0; i < ARRAY_LEN(c_comparisons); i++) {
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

  int node_test_movids_exp_version[100] = {
      26810,  2239,   6460, 7121,  334,   174053, 1964,  177593, 4441,   7008,   2511,  4956,  2925,   6051,   9010,  31364, 2351,
      4444,   3224,   3384, 5833,  213,   168418, 30803, 57274,  121231, 87234,  7842,  27801, 55363,  85736,  90439, 3727,  898,
      115122, 98154,  6345, 27397, 80139, 1836,   51931, 117192, 26680,  104879, 58301, 2436,  106642, 103688, 59141, 942,   3152,
      123,    112804, 3972, 4765,  3771,  26171,  2131,  112175, 1046,   55167,  2349,  47423, 3451,   4334,   1303,  58295, 80906,
      1733,   1238,   1104, 893,   1611,  87529,  3342,  171763, 187595, 80831,  3201,  2035,  42632,  1217,   1273,  3275,  3681,
      89904,  1051,   5466, 1178,  6618,  27722,  26082, 27831,  6306,   162,    6235,  1041,  5404,   3111,   2360};

  int node_test_movids_old_version[100] = {
      26810, 2239,   6460,  7121,  334,   174053, 1964,  177593, 4441,   7008,   2511,   4956,  2925,   6051,   9010,  2351,  31364,
      4444,  3224,   3384,  57274, 213,   168418, 30803, 5833,   115122, 87234,  27801,  7842,  3727,   121231, 55363, 58301, 85736,
      90439, 898,    98154, 6345,  80139, 1836,   51931, 104879, 117192, 26680,  2436,   27397, 103688, 106642, 59141, 942,   3152,
      123,   112804, 3972,  4765,  3771,  26171,  2131,  1046,   112175, 55167,  2349,   47423, 3451,   4334,   58295, 1303,  87529,
      80906, 42632,  1733,  1238,  1104,  893,    1611,  80831,  3342,   171763, 187595, 3201,  2035,   1217,   3275,  1273,  3681,
      89904, 1051,   5466,  1178,  6618,  27831,  26082, 8042,   6306,   162,    6235,   1041,  5404,   3111,   2360,
  };

  int cver_test_movids_exp[100] = {
      26810, 2239, 6460,  7121,  334,   174053, 1964,   177593, 4441,   7008,  2511,   4956,   2925,   6051,   9010,  2351,  31364,
      4444,  3224, 3384,  57274, 213,   168418, 30803,  5833,   115122, 87234, 27801,  7842,   3727,   121231, 55363, 58301, 85736,
      90439, 898,  6345,  80139, 1836,  51931,  104879, 117192, 26680,  98154, 2436,   27397,  103688, 106642, 59141, 942,   3152,
      123,   3972, 4765,  3771,  26171, 2131,   1046,   112175, 55167,  2349,  47423,  3451,   112804, 4334,   58295, 1303,  80906,
      87529, 2035, 42632, 1733,  1238,  1104,   893,    1611,   80831,  3342,  171763, 187595, 3201,   1217,   3275,  1273,  3681,
      1051,  5466, 1178,  89904, 6618,  27831,  26082,  8042,   6306,   162,   1041,   6235,   5404,   3111,   2360,
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
  printf(YELLOW_OUTPUT "\ntest duplicate movids, " RESET_OUTPUT "total recs: %d\n", num_recs);

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
  printf(YELLOW_OUTPUT "\ntest score difference\n" RESET_OUTPUT);

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
