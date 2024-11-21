#ifndef SORTING_H
#define SORTING_H

#ifdef __cplusplus
extern "C" {
#endif

struct thread_vars {
  unsigned int id;
  unsigned int l;
  unsigned int r;
  unsigned int threads;
  struct rating *a;
  struct rating *a_copy;
};

void merg_sort_rating_by_movid(struct rating a[], unsigned int length, unsigned int num_threads);

void merg_sort_rating_by_uid(struct rating a[], unsigned int length, unsigned int num_threads);

void merg_sort_movrec_by_rscore(struct movie_recommendation a[], unsigned int length);

void merg_sort_ws_by_movid(struct weighted_score a[], unsigned int length);

void merg_sort_numratings(struct movie_recommendation a[], unsigned int length);

void bubble_sort_numr(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_numr_rscore(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_numr_rscore_int(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_uid(struct rating movie_recs[], unsigned int num_recs);

#ifdef __cplusplus
}
#endif

#endif