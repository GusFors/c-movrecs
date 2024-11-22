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
  unsigned int val_offset;
  void *(*rec_func)(struct rating a[], unsigned int left, unsigned int right, unsigned int val_offset);
  struct rating *a;
};

void merg_sort_rating_by_movid(struct rating a[], unsigned int length, unsigned int num_threads);

void merg_sort_rating_by_uid(struct rating a[], unsigned int length, unsigned int num_threads);

void *merg_sort_merge_uid(struct rating a[], unsigned int left, unsigned int mid, unsigned int right);

void merg_sort_movrec_by_rscore(struct movie_recommendation a[], unsigned int length);

void merg_sort_ws_by_movid(struct weighted_score a[], unsigned int length);

void merg_sort_numratings(struct movie_recommendation a[], unsigned int length);

void bubble_sort_numr(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_numr_rscore(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_numr_rscore_int(struct movie_recommendation movie_recs[], unsigned int num_recs);

void bubble_sort_uid(struct rating movie_recs[], unsigned int num_recs);

void merge_sort_thread_handler(struct rating a[], unsigned int length, unsigned int num_threads, unsigned int val_offset,
                               void *(*sort_func)(struct rating a[], unsigned int left, unsigned int right, unsigned int val_offset),
                               void *(*merge_func)(struct rating a[], unsigned int, unsigned int, unsigned int, unsigned int,
                                                   void *(*compare_func)(unsigned int *, unsigned int *)));

void *merg_sort_recursion_caller(void *arg);

void *merg_sort_recursion(struct rating a[], unsigned int left, unsigned int right, unsigned int val_offset);

void ins_sort_rating_by_offset(struct rating a[], unsigned int length, unsigned int val_offset);

#ifdef __cplusplus
}
#endif

#endif