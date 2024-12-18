#ifndef CALC_SCORES_H
#define CALC_SCORES_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int weighted_scores_short(struct user_sim *simscores, struct rating *ratings_notseen, unsigned int simlen, unsigned int notseen_cnt,
                                   struct weighted_score *wscores);

unsigned int euclidianscores(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids, unsigned int ulength,
                             struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores);

unsigned int euclidianscores_match(unsigned int userid_a, struct rating *ratings_a, unsigned int numratings_a, unsigned int *uids, unsigned int ulength,
                               struct rating *filtered_ratings, unsigned int filtered_rlength, struct user_sim *simscores);

#ifdef __cplusplus
}
#endif

#endif
