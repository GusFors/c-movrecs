#include "recommender.h"
#include <float.h>
#include <math.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sorting.h"

struct rating r_offsets;
const size_t MOV_ID_OFFSET = (char *)&r_offsets.movie_id - (char *)&r_offsets;
const size_t USER_ID_OFFSET = (char *)&r_offsets.user_id - (char *)&r_offsets;

void *merg_sort_merge_by_offset(struct rating a[], unsigned int left, unsigned int mid, unsigned int right, unsigned int val_offset) {
  unsigned int left_length = mid - left + 1;
  unsigned int right_length = right - mid;

  struct rating *temp_left = malloc((left_length) * sizeof(struct rating));
  struct rating *temp_right = malloc((right_length) * sizeof(struct rating));

  unsigned int i, j, k;

  for (unsigned int i = 0; i < left_length; i++)
    temp_left[i] = a[left + i];

  for (unsigned int i = 0; i < right_length; i++)
    temp_right[i] = a[mid + 1 + i];

  for (i = 0, j = 0, k = left; k <= right; k++) {
    if ((i < left_length) && (j >= right_length || *((int *)((char *)&(temp_left[i]) + val_offset)) <= *((int *)((char *)&(temp_right[j]) + val_offset)))) {
      a[k] = temp_left[i];
      i++;
    } else {
      a[k] = temp_right[j];
      j++;
    }
  }

  free(temp_left);
  free(temp_right);
}

void *merg_sort_recursion_caller(void *arg) {
  struct thread_vars args = *((struct thread_vars *)arg);

  unsigned int thread_id = args.id;
  unsigned int thread_left = args.l;
  unsigned int thread_right = args.r;

  printf("threadid: %d, threadleft: %d, threadright: %d, range: %d\n", thread_id, thread_left, thread_right, thread_right - thread_left);

  struct timespec t1, t2;

  clock_gettime(CLOCK_MONOTONIC, &t1); // CLOCK_PROCESS_CPUTIME_ID?
  args.rec_func(args.a, thread_left, thread_right);

  clock_gettime(CLOCK_MONOTONIC, &t2);
  printf("sort recursion pthread[%d] gettime in  %.17lfms\n", args.id, (double)(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_nsec - t1.tv_nsec) / 1000000.0);
}

void merge_sort_thread_handler(struct rating a[], unsigned int length, unsigned int num_threads, unsigned int val_offset,
                               void *(*sort_func)(struct rating a[], unsigned int left, unsigned int right),
                               void *(*merge_func)(struct rating a[], unsigned int, unsigned int, unsigned int, unsigned int)) {

  pthread_t threads[num_threads];
  unsigned int thread_left = 0;
  unsigned int thread_split = length / num_threads;
  struct thread_vars thread_arg[num_threads];

  for (unsigned int i = 0; i < num_threads; i++, thread_left += thread_split) {
    thread_arg[i].l = thread_left;
    thread_arg[i].r = thread_left + thread_split - 1;

    if (i == (num_threads - 1))
      thread_arg[i].r = length - 1;

    thread_arg[i].threads = num_threads;
    thread_arg[i].id = i;
    thread_arg[i].a = a;
    thread_arg[i].rec_func = sort_func;

    pthread_create(&threads[i], NULL, merg_sort_recursion_caller, &thread_arg[i]);
  }

  for (unsigned int i = 0; i < num_threads; i++)
    pthread_join(threads[i], NULL);

  for (int i = 1; i < num_threads; i++)
    merge_func(a, thread_arg[0].l, thread_arg[i].l - 1, thread_arg[i].r, val_offset);
}

void ins_sort_movid(struct rating a[], unsigned int length) {
  struct rating r;
  for (int i = 0, y = 1; y < length; y++) {
    r = a[y];
    i = y - 1;
    while (i >= 0 && a[i].movie_id > r.movie_id) {
      a[i + 1] = a[i];
      i--;
    }
    a[i + 1] = r;
  }
}

void *merg_sort_movid_r(struct rating a[], unsigned int left, unsigned int right) { // function pointers?
  unsigned int range = right - left;
  if (range < 64) {
    ins_sort_movid(a + left, (range) + 1);
  } else {
    if (left < right) {
      unsigned int m = left + (range) / 2;
      merg_sort_movid_r(a, left, m);
      merg_sort_movid_r(a, m + 1, right);
      merg_sort_merge_by_offset(a, left, m, right, MOV_ID_OFFSET);
    }
  }
}

void merg_sort_rating_by_movid(struct rating a[], unsigned int length, unsigned int num_threads) {
  if (num_threads > 1)
    merge_sort_thread_handler(a, length, num_threads, MOV_ID_OFFSET, merg_sort_movid_r, merg_sort_merge_by_offset);
  else
    merg_sort_movid_r(a, 0, length - 1);
}

void ins_sort_uid(struct rating a[], int length) {
  struct rating r;
  for (int i = 0, y = 1; y < length; y++) {
    r = a[y];
    i = y - 1;
    while (i >= 0 && a[i].user_id > r.user_id) {
      a[i + 1] = a[i];
      i--;
    }
    a[i + 1] = r;
  }
}

void *merg_sort_uid_r(struct rating a[], unsigned int left, unsigned int right) {
  unsigned int range = right - left;
  if (range < 64) {
    ins_sort_uid(a + left, (range) + 1);
    // return 0;
  } else {
    if (left < right) {
      unsigned int mid = left + (right - left) / 2;
      merg_sort_uid_r(a, left, mid);
      merg_sort_uid_r(a, mid + 1, right);
      merg_sort_merge_by_offset(a, left, mid, right, USER_ID_OFFSET);
    }
  }
}

void merg_sort_rating_by_uid(struct rating a[], unsigned int length, unsigned int num_threads) {
  if (num_threads > 1)
    merge_sort_thread_handler(a, length, num_threads, USER_ID_OFFSET, merg_sort_uid_r, merg_sort_merge_by_offset);
  else
    merg_sort_uid_r(a, 0, length - 1);
}

void merg_sort_merge_rscore(struct movie_recommendation a[], unsigned int left, unsigned int mid, unsigned int right) {
  unsigned int left_length = mid - left + 1;
  unsigned int right_length = right - mid;

  struct movie_recommendation *temp_left = malloc((left_length) * sizeof(struct movie_recommendation));
  struct movie_recommendation *temp_right = malloc((right_length) * sizeof(struct movie_recommendation));

  unsigned int i, j, k;

  for (unsigned int i = 0; i < left_length; i++)
    temp_left[i] = a[left + i];

  for (unsigned int i = 0; i < right_length; i++)
    temp_right[i] = a[mid + 1 + i];

  for (i = 0, j = 0, k = left; k <= right; k++) {

    if ((i < left_length) && (j >= right_length || temp_left[i].recommendation_score >= temp_right[j].recommendation_score)) {
      a[k] = temp_left[i];
      i++;

    } else {
      a[k] = temp_right[j];
      j++;
    }
  }

  free(temp_left);
  free(temp_right);
}

void merg_sort_rscore_r(struct movie_recommendation a[], unsigned int left, unsigned int right) {
  if (left < right) {
    unsigned int mid = left + (right - left) / 2;
    merg_sort_rscore_r(a, left, mid);
    merg_sort_rscore_r(a, mid + 1, right);
    merg_sort_merge_rscore(a, left, mid, right);
  }
}

void merg_sort_movrec_by_rscore(struct movie_recommendation a[], unsigned int length) { merg_sort_rscore_r(a, 0, length - 1); }

void merg_sort_merge_movid_numratings(struct movie_recommendation a[], unsigned int left, unsigned int mid, unsigned int right) {
  unsigned int left_length = mid - left + 1;
  unsigned int right_length = right - mid;

  struct movie_recommendation *temp_left = malloc((left_length) * sizeof(struct movie_recommendation));
  struct movie_recommendation *temp_right = malloc((right_length) * sizeof(struct movie_recommendation));

  unsigned int i, j, k;

  for (unsigned int i = 0; i < left_length; i++)
    temp_left[i] = a[left + i];

  for (unsigned int i = 0; i < right_length; i++)
    temp_right[i] = a[mid + 1 + i];

  for (i = 0, j = 0, k = left; k <= right; k++) {
    if ((i < left_length) && (j >= right_length || ((temp_left[i].num_ratings >= temp_right[j].num_ratings &&
                                                     fabs(temp_left[i].recommendation_score - temp_right[j].recommendation_score) < 0.0000001) &&
                                                    temp_left[i].movie_id != temp_right[j].movie_id))) {

      if (((fabs(temp_left[i].recommendation_score - temp_right[j].recommendation_score) < 0.0000001) ||
           temp_left[i].recommendation_score == temp_right[j].recommendation_score) &&
          temp_left[i].movie_id != temp_right[j].movie_id) {
        a[k] = temp_left[i];
        i++;
      }

    } else {
      if (((fabs(temp_left[i].recommendation_score - temp_right[j].recommendation_score) < 0.0000001) ||
           temp_left[i].recommendation_score == temp_right[j].recommendation_score) &&
          temp_left[i].movie_id != temp_right[j].movie_id) {
        a[k] = temp_right[j];
        j++;
      }
    }
  }

  free(temp_left);
  free(temp_right);
}

void merg_sort_movid_numratings_r(struct movie_recommendation a[], unsigned int left, unsigned int right) {
  if (left < right) {
    unsigned int mid = left + (right - left) / 2;
    merg_sort_movid_numratings_r(a, left, mid);
    merg_sort_movid_numratings_r(a, mid + 1, right);
    merg_sort_merge_movid_numratings(a, left, mid, right);
  }
}

void merg_sort_movrec_by_movid_numratings(struct movie_recommendation a[], unsigned int length) { merg_sort_movid_numratings_r(a, 0, length - 1); }

void ins_sort_ws(struct weighted_score a[], int length) {
  struct weighted_score r;
  for (int i = 0, y = 1; y < length; y++) {
    r = a[y];
    i = y - 1;
    while (i >= 0 && a[i].movie_id > r.movie_id) {
      a[i + 1] = a[i];
      i--;
    }
    a[i + 1] = r;
  }
}

void merg_sort_merge_ws(struct weighted_score a[], unsigned int left, unsigned int mid, unsigned int right) {
  unsigned int left_length = mid - left + 1;
  unsigned int right_length = right - mid;

  struct weighted_score *temp_left = malloc((left_length) * sizeof(struct weighted_score));
  struct weighted_score *temp_right = malloc((right_length) * sizeof(struct weighted_score));

  unsigned int i, j, k;

  for (unsigned int i = 0; i < left_length; i++)
    temp_left[i] = a[left + i];

  for (unsigned int i = 0; i < right_length; i++)
    temp_right[i] = a[mid + 1 + i];

  for (i = 0, j = 0, k = left; k <= right; k++) {
    if ((i < left_length) && (j >= right_length || temp_left[i].movie_id <= temp_right[j].movie_id)) {
      a[k] = temp_left[i];
      i++;
    } else {
      a[k] = temp_right[j];
      j++;
    }
  }

  free(temp_left);
  free(temp_right);
}

void merg_sort_ws_by_movid_r(struct weighted_score a[], unsigned int left, unsigned int right) {
  if (left < right) {
    unsigned int range = right - left;
    if (range < 64) {
      ins_sort_ws(a + left, (range) + 1);
      return;
    }

    unsigned int mid = left + (right - left) / 2;
    merg_sort_ws_by_movid_r(a, left, mid);
    merg_sort_ws_by_movid_r(a, mid + 1, right);
    merg_sort_merge_ws(a, left, mid, right);
  }
}

void merg_sort_ws_by_movid(struct weighted_score a[], unsigned int length) { merg_sort_ws_by_movid_r(a, 0, length - 1); }

void bubble_sort_numr(struct movie_recommendation movie_recs[], unsigned int num_recs) {
  unsigned int early_break = 0;

  for (unsigned int i = 0; i < num_recs - 1; i++) {
    early_break = 0;
    for (unsigned int y = 0; y < num_recs - 1; y++) {
      if (movie_recs[y].num_ratings < movie_recs[y + 1].num_ratings) {
        struct movie_recommendation *first_element = &movie_recs[y];
        struct movie_recommendation tempcopy = *first_element;
        struct movie_recommendation *second_element = &movie_recs[y + 1];

        *first_element = *second_element;
        *second_element = tempcopy;
        early_break = 1;
      }
    }

    if (early_break == 0)
      break;
  }
}

void bubble_sort_numr_rscore(struct movie_recommendation movie_recs[], unsigned int num_recs) {
  unsigned int early_break = 0;

  for (unsigned int i = 0; i < num_recs - 1; i++) {
    early_break = 0;
    for (unsigned int y = 0; y < num_recs - 1; y++) {
      if ((fabs(movie_recs[y].recommendation_score - movie_recs[y + 1].recommendation_score)) <= 0.0001 &&
          movie_recs[y].num_ratings < movie_recs[y + 1].num_ratings) {
        struct movie_recommendation *first_element = &movie_recs[y];
        struct movie_recommendation tempcopy = *first_element;
        struct movie_recommendation *second_element = &movie_recs[y + 1];

        *first_element = *second_element;
        *second_element = tempcopy;
        early_break = 1;
      }
    }

    if (early_break == 0)
      break;
  }
}

void bubble_sort_numr_rscore_int(struct movie_recommendation movie_recs[], unsigned int num_recs) {
  unsigned int early_break = 0;

  for (unsigned int i = 0; i < num_recs - 1; i++) {
    early_break = 0;
    for (unsigned int y = 0; y < num_recs - 1; y++) {
      if (((unsigned int)(movie_recs[y].recommendation_score * 10) == (unsigned int)(movie_recs[y + 1].recommendation_score * 10)) &&
          movie_recs[y].num_ratings < movie_recs[y + 1].num_ratings) {
        struct movie_recommendation *first_element = &movie_recs[y];
        struct movie_recommendation tempcopy = *first_element;
        struct movie_recommendation *second_element = &movie_recs[y + 1];

        *first_element = *second_element;
        *second_element = tempcopy;
        early_break = 1;
      }
    }

    if (early_break == 0)
      break;
  }
}

void bubble_sort_uid(struct rating movie_recs[], unsigned int num_recs) {
  unsigned int early_break = 0;

  for (unsigned int i = 0; i < num_recs - 1; i++) {
    early_break = 0;
    for (unsigned int y = 0; y < num_recs - 1; y++) {
      if ((movie_recs[y].user_id > movie_recs[y + 1].user_id)) {
        struct rating *first_element = &movie_recs[y];
        struct rating tempcopy = *first_element;
        struct rating *second_element = &movie_recs[y + 1];
        *first_element = *second_element;
        *second_element = tempcopy;
        early_break = 1;
      }
    }

    if (early_break == 0)
      break;
  }
}

// void *merg_sort_merge_uid(struct rating a[], unsigned int left, unsigned int mid, unsigned int right) {
//   unsigned int left_length = mid - left + 1;
//   unsigned int right_length = right - mid;

//   struct rating *temp_left = malloc((left_length) * sizeof(struct rating));
//   struct rating *temp_right = malloc((right_length) * sizeof(struct rating));

//   unsigned int i, j, k;

//   for (unsigned int i = 0; i < left_length; i++)
//     temp_left[i] = a[left + i];

//   for (unsigned int i = 0; i < right_length; i++)
//     temp_right[i] = a[mid + 1 + i];

//   for (i = 0, j = 0, k = left; k <= right; k++) {
//     if ((i < left_length) && (j >= right_length || temp_left[i].user_id <= temp_right[j].user_id)) {
//       a[k] = temp_left[i];
//       i++;
//     } else {
//       a[k] = temp_right[j];
//       j++;
//     }
//   }

//   free(temp_left);
//   free(temp_right);
// }

// void *merg_sort_merge_movid(struct rating a[], unsigned int left, unsigned int mid, unsigned int right) {
//   unsigned int left_length = mid - left + 1;
//   unsigned int right_length = right - mid;

//   struct rating *temp_left = malloc((left_length) * sizeof(struct rating));
//   struct rating *temp_right = malloc((right_length) * sizeof(struct rating));

//   unsigned int i, j, k;

//   for (unsigned int i = 0; i < left_length; i++)
//     temp_left[i] = a[left + i];

//   for (unsigned int i = 0; i < right_length; i++)
//     temp_right[i] = a[mid + 1 + i];

//   // for (i = 0, j = 0, k = left; k <= right; k++) {
//   //   if ((i < left_length) && (j >= right_length || temp_left[i].movie_id <= temp_right[j].movie_id)) {
//   //     a[k] = temp_left[i];
//   //     i++;
//   //   } else {
//   //     a[k] = temp_right[j];
//   //     j++;
//   //   }
//   // }

//   for (i = 0, j = 0, k = left; k <= right; k++) {
//     if ((i < left_length) && (j >= right_length || *((int *)&(temp_left[i]) + 1) <= *((int *)&(temp_right[j]) + 1))) {
//       a[k] = temp_left[i];
//       i++;
//     } else {
//       a[k] = temp_right[j];
//       j++;
//     }
//   }

//   free(temp_left);
//   free(temp_right);
// }

// void *merg_sort_r(struct rating a[], unsigned int left, unsigned int right,
//                   void *(*rec_sort_func)(struct rating a[], unsigned int left, unsigned int right)) { // function pointers?
//   //  struct thread_vars args = *((struct thread_vars *)arg);
//   unsigned int range = right - left;
//   if (range < 64) {
//     ins_sort_movid(a + left, (range) + 1);
//   } else {
//     if (left < right) {
//       unsigned int m = left + (range) / 2;
//       merg_sort_movid_r(a, left, m);
//       merg_sort_movid_r(a, m + 1, right);
//       merg_sort_merge_by_offset(a, left, m, right, MOV_ID_OFFSET);
//     }
//   }
// }

// void *merg_sort_movid_thread(void *arg) {
//   struct thread_vars args = *((struct thread_vars *)arg);

//   unsigned int thread_id = args.id;
//   unsigned int thread_left = args.l;
//   unsigned int thread_right = args.r;

//   printf("threadid: %d, threadleft: %d, threadright: %d, range: %d\n", thread_id, thread_left, thread_right, thread_right - thread_left);

//   struct timespec t1, t2;

//   clock_gettime(CLOCK_MONOTONIC, &t1);
//   clock_t s1 = clock();
//   merg_sort_movid_r(args.a, thread_left, thread_right);

//   clock_gettime(CLOCK_MONOTONIC, &t2);
//   printf("sort pthread[%d] gettime in  %.17gms\n", args.id, (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000.0);
// }

// void *merg_sort_rating_by_uid_thread(void *arg) {
//   struct thread_vars args = *((struct thread_vars *)arg);

//   unsigned int thread_id = args.id;
//   unsigned int thread_left = args.l;
//   unsigned int thread_right = args.r;

//   printf("threadid: %d, threadleft: %d, threadright: %d, range: %d\n", thread_id, thread_left, thread_right, thread_right - thread_left);

//   struct timespec t1, t2;

//   clock_gettime(CLOCK_MONOTONIC, &t1);
//   clock_t s1 = clock();
//   args.rec_func(args.a, thread_left, thread_right);
//   // merg_sort_uid_r(args.a, thread_left, thread_right);

//   clock_gettime(CLOCK_MONOTONIC, &t2);
//   printf("sort rating_by_uid pthread[%d] gettime in  %.17gms\n", args.id, (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000.0);
//   // return 0;
// }