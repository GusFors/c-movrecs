#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "recommender.h"
#include "readfiles.h"
#include <sys/stat.h>
#include <sys/mman.h>

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

  for (size_t i = 0; i < rfile_size; i++) {
    if (rfile_str[i] == '\n')
      num_lines++;
  }

  printf("num rating lines: %zd\n", num_lines);

  fclose(rfile);
  free(rfile_str);

  return num_lines;
}

ssize_t read_ratings_fast_mem_gl(struct rating *ratings_p) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_END);
  size_t rfile_size = (size_t)ftell(rfile); // check if -1 before casting?

  printf("rating file size in bytes: %ld\n", rfile_size);

  fseek(rfile, 0, SEEK_SET);
  char *rfile_str = malloc(rfile_size + 1);

  fread(rfile_str, rfile_size, 1, rfile);
  fseek(rfile, 0, SEEK_SET);

  char *line = NULL;
  size_t line_length = 0;
  ssize_t readl;
  ssize_t index = -1;

  FILE *rfile_strptr = fmemopen(rfile_str, rfile_size, "r");

  while ((readl = getline(&line, &line_length, rfile_strptr)) != -1) {
    if (index != -1) {
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
    }

    index++;
  }

  fclose(rfile);
  fclose(rfile_strptr);
  free(line);
  printf("\n%zu rating records read.\n", index);

  return index;
}

ssize_t read_ratings_fast_mmap(struct rating *ratings_p) {
  int rfile = open("./data/csv-data/" DATA_PATH "/ratings.csv", O_RDONLY);

  struct stat s;

  if (fstat(rfile, &s) == -1) {
    printf("file stat error\n");
    return -1;
  }

  printf("file size stat: %ld\n", s.st_size);

  char *mapped_rfile = mmap(NULL, (size_t)s.st_size, PROT_READ, MAP_PRIVATE, rfile, 0);
  ssize_t offset = 0;
  ssize_t current_line = -1;
  ssize_t line_size = 0;
  unsigned int index = 0;

  for (ssize_t i = 0; i < s.st_size; i++) {
    if (mapped_rfile[i] != '\n') {
      line_size++;
    } else {
      // or use offset to skip first line
      if (current_line == -1) {
        current_line = 0;
        offset += line_size; // +1 ?
        line_size = 0;
        // printf("last skipped char: %c\n", mapped_rfile[offset]);
        continue;
      }

      unsigned int val_count = 0;
      unsigned int substr_index = 0;
      char userid[64] = "";
      char movieid[64] = "";
      char rating[64] = "";

      // safer with j < line_size but fixed is slightly faster
      for (ssize_t j = 0; j < 64; j++) {
        // if (mapped_rfile[j + offset + 1] == '\n')
        //   break;

        if (mapped_rfile[j + offset] == DELIMITER[0]) {
          val_count++;

          if (val_count == 3)
            break;

          substr_index = 0;
          continue;
        }

        if (val_count == 0)
          userid[substr_index] = mapped_rfile[j + offset];
        else if (val_count == 1)
          movieid[substr_index] = mapped_rfile[j + offset];
        else if (val_count == 2)
          rating[substr_index] = mapped_rfile[j + offset];

        substr_index++;
      }

      ratings_p[index].user_id = (unsigned)strtoul(userid, NULL, 0);
      ratings_p[index].movie_id = (unsigned)strtoul(movieid, NULL, 0);
      ratings_p[index].rating = strtof(rating, NULL);

      // memset(userid, 0, 64);
      // memset(movieid, 0, 64);
      // memset(rating, 0, 64);

      offset += line_size + 1;
      index++;
      line_size = 0;
    }
  }

  // printf("%c", mapped_rfile[j + offset]);
  printf("\n%d rating records read.\n", index);

  return index;
}

ssize_t read_ratings_fast_gl(struct rating *ratings_p) {
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

ssize_t read_ratings_fast(struct rating *ratings_p) {
  FILE *rfile;
  rfile = fopen("./data/csv-data/" DATA_PATH "/ratings.csv", "r");

  if (rfile == NULL) {
    printf("file error\n");
    return -1;
  }

  fseek(rfile, 0, SEEK_END);
  size_t rfile_size = (size_t)ftell(rfile); // check if -1 before casting?

  printf("rating file size in bytes: %ld\n", rfile_size);

  fseek(rfile, 0, SEEK_SET);
  char *rfile_str = malloc(rfile_size + 1);

  fread(rfile_str, rfile_size, 1, rfile);

  ssize_t num_lines = -1;

  for (size_t i = 0; i < rfile_size; i++) {
    if (rfile_str[i] == '\n')
      num_lines++;
  }

  fseek(rfile, 0, SEEK_SET);

  char *line = NULL;
  ssize_t index = -1;

  // char *substrings[num_lines];
  char substr[128] = "";
  unsigned int substr_pos = 0;

  for (size_t i = 0; i < rfile_size; i++) {
    if (rfile_str[i] != '\n') {
      substr[substr_pos] = rfile_str[i];
      substr_pos++;
    } else {
      if (index != -1) {
        size_t length = strlen(substr);

        char userid[64] = "";
        char movieid[64] = "";
        char rating[64] = "";

        unsigned int val_count = 0;
        unsigned int substr_index = 0;

        for (unsigned int j = 0; j < length; j++) {
          if (substr[j] == DELIMITER[0]) {
            val_count++;

            if (val_count == 3)
              break;

            substr_index = 0;
            continue;
          }

          if (val_count == 0)
            userid[substr_index] = substr[j];
          else if (val_count == 1)
            movieid[substr_index] = substr[j];
          else if (val_count == 2)
            rating[substr_index] = substr[j];

          substr_index++;
        }

        ratings_p[index].user_id = (unsigned)strtoul(userid, NULL, 0);
        ratings_p[index].movie_id = (unsigned)strtoul(movieid, NULL, 0);
        ratings_p[index].rating = strtof(rating, NULL);
      }

      index++;

      // substr[substr_pos + 1] = '\0';
      // char *substr_copy = malloc(substr_pos + 1);
      // strncat(substr_copy, substr, substr_pos + 1);
      // substrings[current_line] = substr_copy;

      substr_pos = 0;
      memset(substr, '\0', sizeof(substr));
    }
  }

  printf("rating_buf[%zu]: %d, %d, %lf\n", index, ratings_p[0].user_id, ratings_p[0].movie_id, ratings_p[0].rating);
  printf("rating_buf[%zu]: %d, %d, %lf\n", index, ratings_p[index - 1].user_id, ratings_p[index - 1].movie_id, ratings_p[index - 1].rating);

  fclose(rfile);
  free(line);
  printf("\n%zu rating records read.\n", index);

  return index;
}
