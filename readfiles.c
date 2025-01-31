#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "readfiles.h"

#ifndef DELIMITER
#  define DELIMITER ","
#endif

ssize_t read_ratings_lines(void) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_END);
  size_t rfile_size = (size_t)ftell(rfile);

  printf("rating file size in bytes: %ld\n", rfile_size);

  fseek(rfile, 0, SEEK_SET);
  char *rfile_str = malloc(rfile_size + 1);

  fread(rfile_str, rfile_size, 1, rfile);
  ssize_t num_lines = -1;

  for (uint64_t i = 0; i < rfile_size; i++) {
    if (rfile_str[i] == '\n')
      num_lines++;
  }

  printf("num rating lines: %zd\n", num_lines);

  fclose(rfile);
  free(rfile_str);

  return num_lines;
}

ssize_t read_ratings_fast(struct rating *ratings_p) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_SET);
  char *line = NULL;
  size_t line_length = 0;
  ssize_t readl;
  ssize_t index = -1;

  while ((readl = getline(&line, &line_length, rfile)) != -1) {
    if (index == -1) {
      printf("%s", line);
      index++;
      continue;
    }

    char userid[64] = "";
    char movieid[64] = "";
    char rating[64] = "";

    unsigned int val_count = 0;
    unsigned int substr_index = 0;

    for (unsigned int i = 0; i < line_length; i++) {
      if (line[i] == DELIMITER[0]) {
        val_count++;

        if (val_count == 3)
          break;

        substr_index = 0;
        continue;
      }

      if (val_count == 0)
        userid[substr_index] = line[i];
      else if (val_count == 1)
        movieid[substr_index] = line[i];
      else if (val_count == 2)
        rating[substr_index] = line[i];

      substr_index++;
    }

    ratings_p[index].user_id = (unsigned)strtoul(userid, NULL, 0);
    ratings_p[index].movie_id = (unsigned)strtoul(movieid, NULL, 0);
    ratings_p[index].rating = strtof(rating, NULL);

    index++;
  }

  printf("rating_buf[%zu]: %d, %d, %lf\n", index, ratings_p[0].user_id, ratings_p[0].movie_id, ratings_p[0].rating);
  printf("rating_buf[%zu]: %d, %d, %lf\n", index, ratings_p[index - 1].user_id, ratings_p[index - 1].movie_id, ratings_p[index - 1].rating);

  fclose(rfile);
  free(line);
  printf("\n%zu rating records read.\n", index);

  return index;
}

ssize_t read_movies_lines(void) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/movies.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_SET);
  char *line = NULL;
  size_t line_length = 0;
  ssize_t readl;
  ssize_t num_lines = -1;

  while ((readl = getline(&line, &line_length, rfile)) != -1) {
    num_lines++;
  }

  fclose(rfile);
  free(line);
  printf("\n%zu movies lines read.\n", num_lines);

  return num_lines;
}

ssize_t read_ratings_lines_gl(void) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_SET);

  char *line = NULL;
  size_t line_length = 0;
  ssize_t readl;
  ssize_t num_lines = -1;

  while ((readl = getline(&line, &line_length, rfile)) != -1) {
    num_lines++;
  }

  fclose(rfile);
  free(line);
  printf("\n%zu rating lines read.\n", num_lines);

  return num_lines;
}

ssize_t read_ratings(struct rating *ratings_p) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  int read = 0;
  int records = 0;

  fscanf(rfile, "%*[^\n]");

  while (!feof(rfile)) {
    read = fscanf(rfile, "%d" DELIMITER "%d" DELIMITER "%f" DELIMITER "%*d\n", &ratings_p[records].user_id, &ratings_p[records].movie_id,
                  &ratings_p[records].rating);
    if (read == 3) {
      records++;
    }
  }

  fclose(rfile);
  printf("\n%d rating records read.\n", records);

  return records;
}

ssize_t read_movies(struct movie *movies_p, struct movie_title *mov_titles) {
  FILE *mfile;
  mfile = fopen("./data/csv-data/" DATA_PATH "/movies.csv", "r");

  if (mfile == NULL) {
    printf("file error\n");
    return -1;
  }

  int read = 0;
  unsigned int records = 0;

  fscanf(mfile, "%*[^\n]");

  while (!feof(mfile)) {
    read = fscanf(mfile, "%d" DELIMITER "%255[^\n]", &movies_p[records].movie_id, mov_titles[records].title);
    // read = fscanf(mfile, "%d,%255[^\n]", &movies_p[records].movie_id, movies_p[records].title);

    movies_p[records].num_ratings = 0;
    if (read == 2) {
      records++;
    }
  }

  unsigned int movie_numratings[records];
  for (unsigned int i = 0; i < records; i++)
    movie_numratings[i] = 0;

  fclose(mfile);
  printf("\n%d movie records read.\n", records);

  return records;
}

size_t read_users_num(struct rating *ratings_p, unsigned int rlength) {
  clock_t t1 = clock();

  unsigned int current_uid = 0;
  unsigned int user_cnt = 0;

  for (unsigned int i = 0; i < rlength; i++) {
    current_uid = ratings_p[i].user_id;

    if (user_cnt != current_uid) {
      user_cnt++;
    }
  }

  clock_t t2 = clock() - t1;
  float total = ((float)t2) / CLOCKS_PER_SEC;
  printf("num users from ratings in %f seconds, %.17gms\n", total, total * 1000);
  return user_cnt;
}

size_t read_users_from_ratings(unsigned int *uids, struct rating *ratings_p, unsigned int rlength) {
  clock_t t1 = clock();

  unsigned int current_uid = 0;
  unsigned int user_cnt = 0;

  for (unsigned int i = 0; i < rlength; i++) {
    current_uid = ratings_p[i].user_id;

    if (user_cnt != current_uid) {
      uids[user_cnt] = current_uid;

      user_cnt++;
    }
  }

  clock_t t2 = clock() - t1;
  float total = ((float)t2) / CLOCKS_PER_SEC;
  printf("uids from ratings in %f seconds, %.17gms\n", total, total * 1000);
  printf("\n%d user records read.\n", user_cnt);
  return user_cnt;
}
