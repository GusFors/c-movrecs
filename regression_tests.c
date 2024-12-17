#include "recommender.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "regression_tests.h"

#ifdef FULL_TEST_DATA
#  include "full_test_data.h"
#endif

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

#ifndef WARN_INFO
#  define PRINT_WARN(...) while (0)
#else
#  define PRINT_WARN(...)                                                                                                                                 \
    do {                                                                                                                                                  \
      printf(__VA_ARGS__);                                                                                                                                \
    } while (0)
#endif

#if DATASET == DATASET_SMALL
#  if TEST_SET == NODE_ORIGINAL_TEST_SET
const char *TESTSET_TITLE = "node_original_ver_" DATA_PATH;
#    ifndef FULL_TEST_DATA
struct user_sim similarity_comparisons[] = {
    {1, 0.014652f},   {4, 0.047059f},   {5, 0.047059f},   {6, 0.038835f},   {8, 0.047059f},   {14, 0.137931f},  {15, 0.023529f},  {16, 0.037736f},
    {17, 0.017391f},  {18, 0.023529f},  {19, 0.039216f},  {20, 0.014706f},  {21, 0.028986f},  {22, 0.100000f},  {23, 0.047059f},  {24, 0.075472f},
    {113, 0.036364f}, {115, 0.200000f}, {116, 0.081633f}, {117, 0.026490f}, {118, 0.075472f}, {121, 0.137931f}, {122, 0.023529f}, {124, 0.047059f},
    {125, 0.200000f}, {127, 0.307692f}, {129, 0.200000f}, {131, 0.088889f}, {132, 0.035088f}, {133, 0.075472f}, {135, 0.044944f}, {136, 0.137931f},
    {137, 0.022989f}, {138, 0.800000f}, {140, 0.011331f}, {142, 0.047059f}, {144, 0.100000f}, {146, 0.100000f}, {149, 0.047619f}, {152, 0.051948f},
    {153, 0.056338f}, {155, 0.137931f}, {156, 0.050633f}, {157, 0.047059f}, {158, 0.137931f}, {159, 0.061538f}, {160, 0.017544f}, {161, 0.026667f},
    {210, 0.800000f}, {211, 0.307692f}, {212, 0.100000f}, {215, 0.030303f}, {216, 0.039216f}, {217, 0.015686f}, {219, 0.044444f}, {221, 0.036364f},
    {222, 0.137931f}, {224, 0.021858f}, {225, 0.137931f}, {226, 0.037383f}, {227, 0.075472f}, {229, 0.075472f}, {230, 0.285714f}, {231, 0.307692f},
    {232, 0.017857f}, {233, 0.034188f}, {234, 0.019417f}, {235, 0.047059f}, {236, 0.137931f}, {237, 0.800000f}, {238, 0.047059f}, {239, 0.058824f},
    {240, 0.051282f}, {241, 0.100000f}, {243, 0.137931f}, {244, 0.117647f}, {246, 0.105263f}, {247, 0.051282f}, {249, 0.032787f}, {251, 0.047059f},
    {292, 0.063492f}, {294, 0.043478f}, {296, 0.058824f}, {297, 0.058824f}, {298, 0.068966f}, {300, 0.047059f}, {301, 0.058824f}, {303, 1.000000f},
    {304, 0.015267f}, {305, 0.011236f}, {307, 0.014815f}, {308, 0.800000f}, {309, 0.027972f}, {312, 0.026490f}, {313, 0.076923f}, {314, 0.020513f},
    {433, 0.200000f}, {434, 0.033058f}, {436, 0.024096f}, {437, 0.013841f}, {438, 0.021858f}, {439, 0.058824f}, {440, 0.137931f}, {441, 1.000000f},
    {442, 1.000000f}, {444, 0.047059f}, {445, 0.137931f}, {446, 0.047059f}, {447, 0.075472f}, {448, 0.012461f}, {449, 0.075472f}, {450, 0.075472f},
    {452, 0.040816f}, {453, 0.051282f}, {454, 0.075472f}, {455, 0.047059f}, {457, 1.000000f}, {461, 0.100000f}, {462, 0.055556f}, {463, 0.075472f},
    {464, 0.030075f}, {465, 0.023529f}, {467, 0.047059f}, {469, 0.023121f}, {470, 0.047059f}, {471, 0.058824f}, {474, 0.005634f}, {477, 0.017544f},
    {484, 0.014440f}, {488, 0.017391f}, {489, 0.012270f}, {492, 0.044944f}, {493, 0.307692f}, {495, 0.026490f}, {496, 0.800000f}, {500, 0.051282f},
    {501, 0.057971f}, {503, 0.075472f}, {504, 0.137931f}, {509, 0.050633f}, {510, 0.100000f}, {511, 0.047059f}, {512, 0.047059f}, {514, 0.020725f},
    {515, 0.047059f}, {516, 0.137931f}, {517, 0.018519f}, {518, 0.129032f}, {606, 0.010390f}, {607, 0.026490f}, {608, 0.015504f}, {610, 0.032787f},
};

struct movie_recommendation movie_rec_comparisons[] = {
    {1, 215, 3.8386f},    {2, 110, 3.2786f},     {3, 52, 2.3054f},     {4, 7, 2.7999f},       {5, 49, 3.1401f},     {6, 102, 3.4774f},
    {42, 7, 3.2501f},     {43, 8, 3.1626f},      {44, 46, 2.5835f},    {45, 32, 3.1834f},     {46, 15, 1.7553f},    {47, 203, 3.7254f},
    {48, 68, 3.3135f},    {50, 204, 4.0119f},    {52, 28, 3.3019f},    {54, 3, 2.8514f},      {57, 8, 2.7822f},     {58, 37, 3.6100f},
    {65, 31, 2.9609f},    {66, 9, 2.8457f},      {68, 5, 2.5868f},     {69, 20, 3.9426f},     {70, 55, 3.3891f},    {71, 5, 1.5997f},
    {72, 7, 3.4517f},     {73, 13, 3.0902f},     {74, 8, 4.0199f},     {75, 5, 2.5601f},      {76, 15, 3.1901f},    {78, 3, 3.6931f},
    {79, 17, 2.8775f},    {81, 14, 3.2108f},     {82, 8, 4.0667f},     {85, 7, 3.7944f},      {86, 15, 3.9994f},    {87, 7, 2.5326f},
    {88, 16, 3.1421f},    {92, 8, 2.4595f},      {93, 9, 3.4174f},     {94, 10, 3.9960f},     {95, 84, 3.1376f},    {97, 10, 3.8160f},
    {322, 10, 2.9946f},   {325, 7, 1.9504f},     {326, 3, 4.1585f},    {327, 34, 3.1434f},    {328, 10, 2.6397f},   {329, 108, 3.3618f},
    {330, 6, 3.0726f},    {332, 12, 2.1834f},    {333, 50, 3.8767f},   {334, 5, 4.9361f},     {337, 77, 3.6783f},   {338, 19, 2.4966f},
    {339, 98, 2.9712f},   {340, 5, 3.8266f},     {342, 33, 3.3016f},   {343, 5, 2.6752f},     {344, 161, 2.9727f},  {345, 36, 3.5056f},
    {174053, 4, 4.9277f}, {174055, 13, 2.5914f}, {175303, 9, 4.0571f}, {175485, 3, 1.4386f},  {175569, 5, 3.2222f}, {175661, 3, 2.4451f},
    {176101, 8, 3.1583f}, {176371, 18, 3.3531f}, {177593, 8, 4.9042f}, {177615, 3, 3.0582f},  {177763, 3, 3.0345f}, {177765, 13, 4.4996f},
    {178061, 3, 3.1211f}, {179401, 6, 3.9279f},  {179817, 3, 3.3826f}, {179819, 12, 4.0940f}, {180031, 8, 3.9386f}, {180095, 3, 4.1854f},
    {180985, 5, 4.2070f}, {182715, 3, 3.8600f},  {182823, 4, 3.7497f}, {183897, 5, 3.8083f},  {184471, 4, 2.0319f}, {185029, 4, 2.7593f},
    {187541, 4, 3.3124f}, {187593, 12, 4.0064f}, {187595, 5, 4.6304f}, {188301, 3, 3.6128f},  {3793, 133, 3.9798f}, {3797, 3, 1.6388f},
    {3798, 25, 3.0515f},  {3799, 6, 1.7593f},    {3801, 4, 4.0000f},   {3802, 4, 2.0000f},    {3809, 35, 3.0446f},  {3810, 3, 3.0000f},
    {3811, 7, 4.3513f},   {3812, 12, 3.5985f},   {3813, 3, 4.0793f},   {3814, 9, 4.2951f},    {3819, 3, 3.8447f},   {3821, 16, 1.8890f},
    {3822, 4, 4.1574f},   {3824, 11, 2.6445f},   {3825, 29, 2.6497f},  {3826, 39, 2.2227f},   {3827, 23, 2.1303f},  {3831, 11, 3.6637f},
    {3836, 15, 4.3568f},  {3837, 3, 3.7696f},    {3839, 3, 1.5070f},   {3840, 5, 2.9312f},    {3841, 6, 2.4109f},   {3843, 4, 2.3285f},
    {3844, 9, 3.5866f},   {3846, 4, 4.4779f},    {3852, 13, 3.2627f},  {3858, 5, 3.5620f},    {3861, 19, 2.4613f},  {3863, 45, 1.9590f},
    {3864, 5, 3.3245f},   {3865, 4, 3.5453f},    {3868, 42, 3.5412f},  {3869, 26, 2.5773f},   {3871, 5, 3.9756f},   {3873, 7, 3.2901f},
    {3877, 9, 1.8379f},   {3882, 34, 2.5734f},   {3884, 3, 1.8793f},   {3889, 6, 2.1492f},    {3893, 19, 3.3363f},  {3896, 8, 3.3912f},
    {3897, 83, 3.5424f},  {3908, 3, 1.5119f},    {3909, 4, 1.9613f},   {3910, 20, 3.7027f},   {3911, 53, 3.6519f},  {3915, 6, 3.9229f},
    {3916, 41, 3.4765f},  {3917, 15, 3.8723f},   {3918, 9, 3.5241f},   {3925, 6, 3.7426f},    {3927, 6, 2.8726f},   {3928, 4, 4.0787f},
    {3930, 4, 2.6740f},   {3937, 4, 2.0955f},    {3943, 8, 3.4112f},
};
#    endif
#  else // C exp test set
const char *TESTSET_TITLE = "c_version_exp_" DATA_PATH;
struct user_sim similarity_comparisons[] = {
    {1, 0.014652f},   {4, 0.047059f},   {5, 0.047059f},   {6, 0.038835f},   {8, 0.047059f},   {14, 0.137931f},  {15, 0.023529f},  {16, 0.037736f},
    {71, 0.075472f},  {72, 0.038462f},  {73, 0.307692f},  {74, 0.024096f},  {75, 1.000000f},  {80, 0.075472f},  {82, 0.500000f},  {84, 0.008547f},
    {85, 0.047059f},  {88, 0.064516f},  {89, 0.042553f},  {91, 0.121212f},  {93, 0.022989f},  {95, 0.137931f},  {96, 0.036364f},  {98, 0.137931f},
    {100, 0.075472f}, {101, 0.047059f}, {103, 0.017467f}, {104, 0.012658f}, {105, 0.016807f}, {109, 0.074074f}, {110, 0.039216f}, {111, 0.105263f},
    {113, 0.036364f}, {115, 0.200000f}, {116, 0.081633f}, {117, 0.026490f}, {118, 0.075472f}, {121, 0.137931f}, {122, 0.023529f}, {124, 0.047059f},
    {315, 0.137931f}, {317, 0.075472f}, {318, 0.024540f}, {319, 0.047059f}, {320, 0.200000f}, {323, 0.058824f}, {324, 0.137931f}, {325, 0.105263f},
    {326, 0.030303f}, {328, 0.016949f}, {330, 0.022989f}, {332, 0.028169f}, {333, 0.137931f}, {334, 0.075472f}, {335, 0.137931f}, {336, 0.075472f},
    {344, 1.000000f}, {345, 0.137931f}, {346, 0.061538f}, {348, 0.047059f}, {359, 0.038462f}, {361, 0.054795f}, {362, 0.033058f}, {366, 0.075472f},
    {367, 0.014706f}, {368, 0.014388f}, {369, 0.051282f}, {372, 0.031008f}, {373, 0.047059f}, {376, 0.051282f}, {377, 0.060606f}, {378, 0.038462f},
    {379, 0.137931f}, {380, 0.022472f}, {381, 0.030769f}, {382, 0.019417f}, {385, 0.036364f}, {387, 0.017857f}, {390, 0.058824f}, {391, 0.015209f},
    {395, 0.307692f}, {398, 0.047059f}, {399, 0.075472f}, {400, 0.047059f}, {402, 0.075472f}, {403, 0.071429f}, {404, 0.075472f}, {405, 0.057971f},
    {408, 0.047059f}, {409, 0.137931f}, {410, 0.075472f}, {411, 0.029851f}, {413, 0.033058f}, {414, 0.006061f}, {416, 0.100000f}, {417, 0.058824f},
    {418, 0.058824f}, {419, 0.029851f}, {420, 0.058824f}, {421, 0.026846f}, {522, 0.042553f}, {523, 0.030303f}, {524, 0.045977f}, {525, 0.024390f},
    {527, 0.059701f}, {532, 0.047059f}, {533, 0.500000f}, {534, 0.022472f}, {536, 0.047059f}, {537, 0.800000f}, {539, 0.100000f}, {541, 0.047059f},
    {542, 0.075472f}, {549, 0.800000f}, {552, 0.044944f}, {553, 0.047059f}, {554, 0.015152f}, {555, 0.011236f}, {557, 0.075472f}, {560, 0.044944f},
    {561, 0.017621f}, {562, 0.011905f}, {563, 0.075472f}, {565, 0.047059f}, {566, 0.047059f}, {570, 0.058824f}, {571, 0.057971f}, {572, 0.016667f},
    {576, 0.051282f}, {577, 0.018519f}, {579, 0.036364f}, {580, 0.029197f}, {581, 0.026846f}, {586, 0.200000f}, {587, 0.028986f}, {588, 0.075472f},
    {589, 0.047059f}, {590, 0.010363f}, {593, 0.043011f}, {594, 0.047059f}, {596, 0.036697f}, {597, 0.011869f}, {599, 0.009780f}, {600, 0.014760f},
    {601, 0.029851f}, {602, 0.036364f}, {603, 0.015209f}, {605, 0.800000f}, {606, 0.010390f}, {607, 0.026490f}, {608, 0.015504f}, {610, 0.032787f},
};
#  endif
#elif TEST_SET == NODE_ORIGINAL_DIFF_USER_SET
const char *TESTSET_TITLE = "node_original_diff_user_ver_" DATA_PATH;
#  ifndef FULL_TEST_DATA
// not sorted by score
struct movie_recommendation movie_rec_comparisons[] = {
    {1, 215, 4.0279f},   {2, 110, 3.6007f},   {3, 52, 3.3538f},    {5, 49, 3.0367f},    {6, 102, 4.0441f},   {7, 54, 3.1219f},    {10, 132, 3.4433f},
    {11, 70, 3.8961f},   {16, 82, 3.8108f},   {17, 67, 3.8556f},   {19, 88, 2.8388f},   {21, 89, 3.4195f},   {22, 36, 3.3340f},   {25, 76, 3.8986f},
    {29, 38, 3.7438f},   {31, 38, 3.2345f},   {32, 177, 3.9661f},  {34, 128, 3.7271f},  {36, 67, 3.9643f},   {39, 104, 3.1444f},  {44, 46, 2.4957f},
    {45, 32, 3.6114f},   {47, 203, 3.9602f},  {48, 68, 3.1261f},   {50, 204, 4.2022f},  {52, 28, 3.6169f},   {58, 37, 4.1429f},   {60, 34, 3.1751f},
    {62, 80, 3.6617f},   {65, 31, 2.6885f},   {70, 55, 3.3751f},   {95, 84, 3.2351f},   {104, 99, 3.3888f},  {107, 26, 3.2152f},  {110, 237, 4.1295f},
    {111, 104, 4.1974f}, {112, 46, 3.6609f},  {135, 31, 2.9326f},  {141, 86, 3.5399f},  {145, 51, 3.1054f},  {150, 201, 3.7957f}, {151, 44, 3.5160f},
    {153, 137, 3.1300f}, {158, 62, 2.9511f},  {160, 57, 2.8036f},  {161, 103, 3.4826f}, {163, 66, 3.3084f},  {165, 144, 3.6711f}, {168, 54, 3.1950f},
    {170, 35, 3.2052f},  {172, 53, 2.3220f},  {173, 62, 3.0369f},  {180, 36, 3.4808f},  {185, 112, 3.1039f}, {186, 48, 2.8750f},  {193, 35, 2.3538f},
    {196, 45, 2.6457f},  {198, 28, 3.2281f},  {204, 30, 2.5090f},  {208, 115, 2.7590f}, {216, 49, 3.3960f},  {224, 40, 3.4710f},  {225, 52, 3.4753f},
    {231, 133, 2.9638f}, {235, 70, 3.8424f},  {236, 45, 3.5567f},  {246, 29, 4.3189f},  {252, 43, 3.3983f},  {253, 109, 3.3672f}, {256, 35, 3.1634f},
    {260, 251, 4.1411f}, {261, 42, 3.7008f},  {265, 33, 4.2750f},  {266, 68, 3.4797f},  {272, 31, 3.7804f},  {273, 28, 3.1040f},  {277, 38, 3.4463f},
    {282, 40, 3.4571f},  {292, 101, 3.5345f}, {293, 133, 4.0978f}, {296, 307, 4.1366f}, {300, 81, 3.5700f},  {303, 32, 2.5264f},  {315, 38, 3.0049f},
    {316, 140, 3.3699f}, {317, 81, 3.3214f},  {318, 317, 4.4212f}, {327, 34, 3.0979f},  {329, 108, 3.4936f}, {333, 50, 3.6939f},  {337, 77, 3.9655f},
    {339, 98, 3.5410f},  {342, 33, 3.1597f},  {344, 161, 3.1761f}, {345, 36, 3.9073f},  {348, 30, 3.8256f},  {349, 110, 3.7642f}, {350, 57, 3.4934f},
    {353, 64, 3.2340f},  {355, 42, 2.5223f},  {356, 329, 4.1800f}, {357, 103, 3.6633f}, {362, 34, 3.4410f},  {364, 172, 3.9455f}, {367, 157, 3.2044f},
    {368, 74, 3.2880f},  {370, 58, 3.1193f},  {374, 25, 1.9631f},  {376, 40, 3.4774f},  {377, 171, 3.5633f}, {380, 178, 3.6037f}, {410, 84, 3.1623f},
    {413, 26, 2.6833f},  {420, 59, 2.7444f},  {431, 42, 3.7039f},  {432, 55, 2.8176f},  {434, 101, 3.0006f}, {435, 63, 2.4112f},  {440, 76, 3.5846f},
    {441, 42, 3.6106f},  {442, 81, 3.0514f},  {454, 101, 3.6350f}, {455, 37, 2.6281f},  {457, 190, 4.1226f}, {466, 60, 2.9611f},  {471, 40, 4.0466f},
    {474, 70, 3.7354f},  {475, 25, 4.2984f},  {480, 238, 3.8326f}, {485, 53, 3.1005f},  {494, 41, 3.4977f},  {497, 43, 4.2177f},  {500, 144, 3.4717f},
    {508, 66, 3.5529f},  {509, 61, 3.4706f},  {520, 69, 2.9381f},  {524, 34, 3.5323f},  {527, 220, 4.3345f}, {529, 40, 3.9999f},  {539, 106, 3.7306f},
    {541, 124, 4.1171f}, {543, 41, 2.9691f},  {551, 93, 3.5428f},  {552, 61, 3.0992f},  {553, 65, 3.7370f},  {555, 65, 3.6683f},  {585, 36, 2.8373f},
};

struct user_sim similarity_comparisons[] = {
    {1, 0.055556f},   {4, 0.062500f},   {6, 0.142857f},   {7, 0.285714f},   {9, 0.200000f},   {10, 0.075472f},  {11, 1.000000f},  {12, 0.200000f},
    {13, 0.500000f},  {14, 0.500000f},  {15, 0.153846f},  {19, 0.018182f},  {20, 0.088889f},  {21, 0.105263f},  {22, 0.800000f},  {24, 0.444444f},
    {25, 0.500000f},  {26, 1.000000f},  {27, 0.250000f},  {28, 0.053333f},  {29, 0.800000f},  {59, 0.200000f},  {60, 1.000000f},  {62, 0.363636f},
    {63, 0.083333f},  {64, 0.026846f},  {65, 0.800000f},  {66, 0.058824f},  {67, 0.666667f},  {68, 0.016598f},  {69, 0.055556f},  {70, 0.307692f},
    {71, 0.250000f},  {72, 0.400000f},  {73, 0.800000f},  {74, 0.166667f},  {75, 0.800000f},  {76, 0.444444f},  {77, 0.500000f},  {78, 0.571429f},
    {80, 0.400000f},  {81, 1.000000f},  {82, 0.083333f},  {83, 0.800000f},  {84, 0.333333f},  {157, 0.071429f}, {158, 0.105263f}, {159, 0.235294f},
    {160, 0.026667f}, {164, 0.500000f}, {165, 0.500000f}, {166, 0.571429f}, {167, 0.129032f}, {169, 0.078431f}, {170, 0.500000f}, {171, 0.333333f},
    {172, 0.200000f}, {174, 1.000000f}, {176, 0.500000f}, {177, 0.037383f}, {178, 0.571429f}, {179, 0.500000f}, {180, 0.307692f}, {181, 1.000000f},
    {182, 0.018182f}, {183, 1.000000f}, {185, 0.333333f}, {186, 0.031250f}, {187, 0.181818f}, {189, 1.000000f}, {190, 1.000000f}, {191, 1.000000f},
    {192, 1.000000f}, {193, 0.142857f}, {194, 0.444444f}, {195, 0.076923f}, {196, 0.500000f}, {197, 0.500000f}, {198, 0.125000f}, {199, 0.030769f},
    {200, 0.078431f}, {201, 0.071429f}, {202, 0.250000f}, {203, 0.800000f}, {204, 0.500000f}, {475, 0.333333f}, {477, 0.062500f}, {478, 0.800000f},
    {479, 0.090909f}, {480, 0.025000f}, {482, 0.190476f}, {483, 0.051282f}, {484, 0.057971f}, {486, 0.333333f}, {487, 0.444444f}, {488, 0.800000f},
    {489, 0.026490f}, {490, 0.800000f}, {491, 0.500000f}, {492, 0.142857f}, {493, 0.100000f}, {494, 0.500000f}, {495, 0.072727f}, {497, 1.000000f},
    {500, 0.062500f}, {503, 0.800000f}, {505, 0.500000f}, {508, 0.800000f}, {509, 0.250000f}, {510, 0.086957f}, {511, 0.333333f}, {512, 0.200000f},
    {514, 0.129032f}, {515, 0.500000f}, {516, 1.000000f}, {517, 0.033333f}, {518, 0.800000f}, {520, 0.044444f}, {521, 1.000000f}, {522, 0.181818f},
    {523, 0.800000f}, {524, 0.333333f}, {525, 0.148148f}, {526, 1.000000f}, {527, 0.076923f}, {529, 0.500000f}, {531, 0.500000f}, {532, 0.500000f},
    {533, 0.500000f}, {534, 0.173913f}, {535, 0.027778f}, {540, 0.800000f}, {541, 0.200000f}, {591, 0.050000f}, {592, 0.333333f}, {593, 0.129032f},
    {594, 0.078431f}, {595, 0.500000f}, {596, 0.121212f}, {597, 0.023810f}, {599, 0.019417f}, {600, 0.023952f}, {601, 0.444444f}, {602, 1.000000f},
    {603, 0.010101f}, {604, 0.166667f}, {605, 0.333333f}, {606, 0.043011f}, {607, 0.125000f}, {608, 0.019324f}, {609, 1.000000f}, {610, 0.045455f},
};
#  endif // external full test data include
#elif DATASET == DATASET_ORIGINAL
const char *TESTSET_TITLE = "ORIGINAL_NOT_IMPLEMENTED " DATA_PATH;
struct user_sim similarity_comparisons[] = {0};
struct movie_recommendation movie_rec_comparisons[] ={0}
#else
const char *TESTSET_TITLE = "NOT_IMPLEMENTED " DATA_PATH;
#  ifndef FULL_TEST_DATA
struct user_sim similarity_comparisons[] = {0};
struct movie_recommendation movie_rec_comparisons[] = {0};
#  endif
#endif

int test_movie_recommendation_tests(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length, unsigned int user_id) {
  printf(YELLOW_OUTPUT "\ntest - compare movie recommendations - %s\n" RESET_OUTPUT, TESTSET_TITLE);

#ifndef MATCHING_TESTS
  printf("No matching movie recommendation tests for current dataset, ignoring test\n");
  return RETURN_FAIL;
#endif

  if (user_id != DEFAULT_USER) {
    printf("Ignoring movie test, id doesn't match\n");
    return RETURN_FAIL;
  }

  test_compare_movrec_movids(calculated_movie_recs, calculated_mov_rec_length);
  test_compare_movrec_numratings(calculated_movie_recs, calculated_mov_rec_length);
  test_compare_movrec_scores(calculated_movie_recs, calculated_mov_rec_length);
  printf("\n");

  return 0;
}

int test_compare_movrec_movids(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length) {
  unsigned int num_exact_matchings_ids = 0;
  unsigned int num_warnings = 0;

#ifdef FULL_TEST_DATA
  for (size_t i = 0; i < ARRAY_LEN(movie_rec_comparisons); i++) {
    if (movie_rec_comparisons[i].movie_id != calculated_movie_recs[i].movie_id)
      printf("" FAIL_RED " full comparison, movie id doesn't match: expected movid: %d, actual: %d\n", movie_rec_comparisons[i].movie_id,
             calculated_movie_recs[i].movie_id);
  }
#endif

  for (size_t i = 0; i < ARRAY_LEN(movie_rec_comparisons); i++) {
    for (size_t j = 0; j < calculated_mov_rec_length; j++) {
      if (movie_rec_comparisons[i].movie_id == calculated_movie_recs[j].movie_id)
        num_exact_matchings_ids++;
    }
  }

  printf("num_exact_matchings_movids: %d/%lu ", num_exact_matchings_ids, ARRAY_LEN(movie_rec_comparisons));

  if (num_exact_matchings_ids < ARRAY_LEN(movie_rec_comparisons)) {
    printf("" FAIL_RED "\n");
    return RETURN_FAIL;
  }

  num_warnings ? printf(PASS_GREEN " - %d warnings\n", num_warnings) : printf(PASS_GREEN "\n");
  return RETURN_SUCCESS;
}

int test_compare_movrec_numratings(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length) {
  unsigned int num_matching_numratings = 0;
  unsigned int num_warnings = 0;

  for (size_t i = 0; i < ARRAY_LEN(movie_rec_comparisons); i++) {
    for (size_t j = 0; j < calculated_mov_rec_length; j++) {
      if (movie_rec_comparisons[i].movie_id == calculated_movie_recs[j].movie_id) {
        if (movie_rec_comparisons[i].num_ratings == calculated_movie_recs[j].num_ratings)
          num_matching_numratings++;
        else
          printf("" FAIL_RED " numratings diff for movieid: %d, expected numratings: %d, actual: %d\n", movie_rec_comparisons[i].movie_id,
                 movie_rec_comparisons[i].num_ratings, calculated_movie_recs[j].num_ratings);
      }
    }
  }

  printf("num_matchings_numratings: %d/%lu ", num_matching_numratings, ARRAY_LEN(movie_rec_comparisons));

  if (num_matching_numratings < ARRAY_LEN(movie_rec_comparisons)) {
    printf("" FAIL_RED "\n");
    return RETURN_FAIL;
  }

  num_warnings ? printf(PASS_GREEN " - %d warnings\n", num_warnings) : printf(PASS_GREEN "\n");
  return RETURN_SUCCESS;
}

int test_compare_movrec_scores(struct movie_recommendation *calculated_movie_recs, unsigned int calculated_mov_rec_length) {
  unsigned int num_matchings_scores = 0;
  unsigned int num_warnings = 0;

  for (size_t i = 0; i < ARRAY_LEN(movie_rec_comparisons); i++) {
    for (size_t j = 0; j < calculated_mov_rec_length; j++) {
      if (movie_rec_comparisons[i].movie_id == calculated_movie_recs[j].movie_id) {
        double score_diff = fabs(movie_rec_comparisons[i].recommendation_score - calculated_movie_recs[j].recommendation_score);

        if (score_diff > 0.005) {
          printf("" FAIL_RED " recommendation score diff for movieid: %d, expected score: %f, actual: %f\n", movie_rec_comparisons[i].movie_id,
                 movie_rec_comparisons[i].recommendation_score, calculated_movie_recs[j].recommendation_score);
          continue;
        }

        if (score_diff < 0.0001) {
          num_matchings_scores++;
          continue;
        }

        if (score_diff > 0.001) {
          PRINT_WARN("" WARNING_YELLOW " small recommendation score diff for userid: %d, expected score: %f, actual: %f\n",
                     movie_rec_comparisons[i].movie_id, movie_rec_comparisons[i].recommendation_score, calculated_movie_recs[j].recommendation_score);
          num_warnings++;
          num_matchings_scores++;
          continue;
        }
      }
    }
  }

  printf("num_matchings_movrec_scores: %d/%lu ", num_matchings_scores, ARRAY_LEN(movie_rec_comparisons));

  if (num_matchings_scores < ARRAY_LEN(movie_rec_comparisons)) {
    printf("" FAIL_RED "\n");
    return RETURN_FAIL;
  }

  num_warnings ? printf(PASS_GREEN " - %d warnings\n", num_warnings) : printf(PASS_GREEN "\n");
  return RETURN_SUCCESS;
}

int test_compare_sim_scores(struct user_sim *calculated_sim_scores, unsigned int calculated_sim_scores_length, unsigned int user_id) {
  printf(YELLOW_OUTPUT "\ntest compare similarity scores %s\n" RESET_OUTPUT, TESTSET_TITLE);

#ifndef MATCHING_TESTS
  printf("No matching tests for current dataset, ignoring test\n");
  return RETURN_FAIL;
#endif

  if (user_id != DEFAULT_USER) {
    printf("Ignoring similarity test, id doesn't match\n");
    return RETURN_FAIL;
  }

  unsigned int num_matchings_scores = 0;
  unsigned int num_warnings = 0;

#ifdef FULL_TEST_DATA
  unsigned int num_exact_matchings_ids = 0;
  for (size_t i = 0; i < ARRAY_LEN(similarity_comparisons); i++) {
    if (similarity_comparisons[i].user_id == calculated_sim_scores[i].user_id) {
      num_exact_matchings_ids++;
      continue;
    }

    // higher ids seem to be offset one or a few steps compared to original?
    printf("" FAIL_RED " simscore userid does not match: expected userid: %d, actual: %d\n", similarity_comparisons[i].user_id,
           calculated_sim_scores[i].user_id);
  }

  printf("%s num_exact_matchings_uids: %d/%lu \n", TESTSET_TITLE, num_exact_matchings_ids, ARRAY_LEN(similarity_comparisons));

  if (num_exact_matchings_ids < ARRAY_LEN(similarity_comparisons))
    printf("%s similarity score test: " FAIL_RED, TESTSET_TITLE);

#endif

  for (size_t i = 0; i < ARRAY_LEN(similarity_comparisons); i++) {
    for (size_t j = 0; j < calculated_sim_scores_length; j++) {
      if (similarity_comparisons[i].user_id == calculated_sim_scores[j].user_id) {
        double score_diff = fabs(similarity_comparisons[i].simscore - calculated_sim_scores[j].simscore);

        if (score_diff > 0.005) {
          printf("" FAIL_RED " simscore diff for userid: %d, expected simscore: %f, actual: %f\n", similarity_comparisons[i].user_id,
                 similarity_comparisons[i].simscore, calculated_sim_scores[j].simscore);
          continue;
        }

        if (score_diff < 0.0001) {
          num_matchings_scores++;
          continue;
        }

        if (score_diff > 0.001) {
          PRINT_WARN("" WARNING_YELLOW " small simscore diff for userid: %d, expected simscore: %f, actual: %f\n", similarity_comparisons[i].user_id,
                     similarity_comparisons[i].simscore, calculated_sim_scores[j].simscore);
          num_warnings++;
          num_matchings_scores++;
          continue;
        }
      }
    }
  }

  printf("%s num matchings_simscores: %d/%lu \n", TESTSET_TITLE, num_matchings_scores, ARRAY_LEN(similarity_comparisons));
  // printf(YELLOW_OUTPUT "\ntest compare similarity scores %s\n" RESET_OUTPUT, TESTSET_TITLE);

  if (num_matchings_scores < ARRAY_LEN(similarity_comparisons)) {
    printf("%s similarity score test: " FAIL_RED, TESTSET_TITLE);
    return RETURN_FAIL;
  }

  printf("similarity score test: ");
  num_warnings ? printf(PASS_GREEN " - %d warnings", num_warnings) : printf("%d/%lu " PASS_GREEN, num_matchings_scores, ARRAY_LEN(similarity_comparisons));
  return RETURN_SUCCESS;
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

  return pass;
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

// struct user_sim *usersim_comparisons = similarity_comparisons;
// unsigned int usersim_len;

// struct movie_recommendation *movierec_comparisons;
// unsigned int movierec_len;

void node_old_comparisons(struct test_comparison_data *t) {
  // struct test_comparison_data t;
  //  printf("test comp simlen: %lu\n", ARRAY_LEN(similarity_comparisons));
  t->title = "node_original";

  struct user_sim node_old_comparison_scores[] = {
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
      {185, 0.047059f}, {186, 0.061538f}, {187, 0.027972f}, {188, 0.047059f}, {189, 0.058824f}, {190, 0.047059f}, {191, 0.047059f}, {193, 0.075472f},
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
      {373, 0.047059f}, {376, 0.051282f}, {377, 0.057143f}, {378, 0.038462f}, {379, 0.137931f}, {380, 0.022472f}, {381, 0.030769f}, {382, 0.019417f},
      {385, 0.036364f}, {387, 0.017857f}, {390, 0.058824f}, {391, 0.015209f}, {395, 0.307692f}, {398, 0.047059f}, {399, 0.075472f}, {400, 0.047059f},
      {402, 0.075472f}, {403, 0.071429f}, {404, 0.075472f}, {405, 0.057971f}, {408, 0.047059f}, {409, 0.137931f}, {410, 0.075472f}, {411, 0.029851f},
      {413, 0.033058f}, {414, 0.006061f}, {416, 0.100000f}, {417, 0.058824f}, {418, 0.058824f}, {419, 0.029851f}, {420, 0.058824f}, {421, 0.026846f},
      {422, 0.047059f}, {424, 0.066667f}, {425, 0.067797f}, {426, 0.029851f}, {427, 0.200000f}, {428, 0.200000f}, {430, 0.500000f}, {432, 0.019048f},
      {433, 0.200000f}, {434, 0.033058f}, {436, 0.024096f}, {437, 0.013841f}, {438, 0.021858f}, {439, 0.058824f}, {440, 0.137931f}, {441, 1.000000f},
      {442, 1.000000f}, {444, 0.047059f}, {445, 0.137931f}, {446, 0.047059f}, {447, 0.075472f}, {448, 0.012461f}, {449, 0.075472f}, {450, 0.075472f},
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

  size_t score_bytes = sizeof(struct user_sim) * ARRAY_LEN(node_old_comparison_scores);
  struct user_sim *score_copy = malloc(score_bytes);
  memcpy(score_copy, node_old_comparison_scores, score_bytes);

  t->similarity_scores = score_copy;
  t->simscores_len = ARRAY_LEN(node_old_comparison_scores);

  struct movie_recommendation node_old_recs[] = {
      [0] = {.movie_id = 26810, .num_ratings = 3, .recommendation_score = 4.9691f},
      [1] = {.movie_id = 2239, .num_ratings = 6, .recommendation_score = 4.9557f},
      [2] = {.movie_id = 98154, .num_ratings = 4, .recommendation_score = 4.7583f},
      [3] = {.movie_id = 100507, .num_ratings = 4, .recommendation_score = 3.9442f},
      [4] = {.movie_id = 307, .num_ratings = 24, .recommendation_score = 3.9752f},
      [5] = {.movie_id = 68954, .num_ratings = 105, .recommendation_score = 3.9701f},
  };

  size_t movrec_bytes = sizeof(struct movie_recommendation) * ARRAY_LEN(node_old_recs);
  struct movie_recommendation *movrec_copy = malloc(movrec_bytes);
  memcpy(movrec_copy, node_old_recs, movrec_bytes);

  t->movie_recommendations = movrec_copy;
  t->movrec_len = ARRAY_LEN(node_old_recs);
}

int test_compare_sim_scores_n(struct user_sim *calculated_sim_scores, struct test_comparison_data *comparison_scores) {
  printf(YELLOW_OUTPUT "\ntest compare similarity scores %s\n" RESET_OUTPUT, comparison_scores->title);

  unsigned int num_exact_matchings_ids = 0;
  unsigned int num_matchings_scores = 0;
  unsigned int num_warnings = 0;

  for (size_t i = 0; i < comparison_scores->simscores_len; i++) {
    if (comparison_scores->similarity_scores[i].user_id == calculated_sim_scores[i].user_id) {
      num_exact_matchings_ids++;
      continue;
    }

    printf("" FAIL_RED " simscore userid does not match: expected userid: %d, actual: %d\n", comparison_scores->similarity_scores[i].user_id,
           calculated_sim_scores[i].user_id);
  }

  for (size_t i = 0; i < comparison_scores->simscores_len; i++) {
    double score_diff = fabs(comparison_scores->similarity_scores[i].simscore - calculated_sim_scores[i].simscore);

    if (score_diff > 0.005) {
      printf("" FAIL_RED " simscore diff for userid: %d, expected simscore: %f, actual: %f\n", comparison_scores->similarity_scores[i].user_id,
             comparison_scores->similarity_scores[i].simscore, calculated_sim_scores[i].simscore);
      continue;
    }

    if (score_diff < 0.0001) {
      num_matchings_scores++;
      continue;
    }

    if (score_diff > 0.001) {
      PRINT_WARN("" WARNING_YELLOW " small simscore diff for userid: %d, expected simscore: %f, actual: %f\n",
                 comparison_scores->similarity_scores[i].user_id, comparison_scores->similarity_scores[i].simscore, calculated_sim_scores[i].simscore);
      num_warnings++;
      num_matchings_scores++;
      continue;
    }
  }

  printf("node_old num_exact_matchings_uids: %d/%lu \n", num_exact_matchings_ids, comparison_scores->simscores_len);
  printf("node_old num matchings_simscores:  %d/%lu \n", num_matchings_scores, comparison_scores->simscores_len);

  // printf("" WARNING_YELLOW " low amount of exactly matching uids\n");
  if (num_exact_matchings_ids < comparison_scores->simscores_len) {
    printf("node_old similarity score test: " FAIL_RED);
    return RETURN_FAIL;
  }

  printf("similarity score test: ");
  num_warnings ? printf(PASS_GREEN " - %d warnings", num_warnings) : printf(PASS_GREEN);
  return RETURN_SUCCESS;
}
