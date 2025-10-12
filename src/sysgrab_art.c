#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysgrab_art.h"

#define MAX_LEN 256

Art *create_art(size_t lines, size_t maximum_line_length, char **art_array) {
  Art *art = malloc(sizeof(Art));
  if (art == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  art->lines = lines;
  art->maximum_line_length = maximum_line_length;
  art->art_array = art_array;

  return art;
}

void free_art_array(size_t lines, char **art_array) {
  for (size_t i = 0; i < lines; i++) {
    free(art_array[i]);
  }
  free(art_array);
}

void free_art(Art *art) {
  free_art_array(art->lines, art->art_array);
  free(art);
}

bool generate_art_file(const char *file_path) {
  FILE *fp = fopen(file_path, "w");
  if (fp == NULL) {
    perror("Failed to generate art file");
    return false;
  }
  fclose(fp);
  return true;
}

FILE *open_art_file(const char *file_path) {
  FILE *fp = fopen(file_path, "r");
  if (fp == NULL) {
    if (!generate_art_file(file_path)) {
      perror("Failed to generate art file");
      return NULL;
    }

    fp = fopen(file_path, "r");
    if (fp == NULL) {
      perror("Failed to open art file");
      return NULL;
    }
  }

  return fp;
}

Art *get_art(const char *file_path) {
  size_t lines = 0;
  size_t maximum_line_length = 0;
  char **art_array = NULL;
  Art *art = NULL;

  char temp[MAX_LEN];

  FILE *fp = open_art_file(file_path);
  if (fp == NULL) {
    perror("Failed to open art file");
    return NULL;
  }

  while (fgets(temp, MAX_LEN, fp)) {
    // Remove newline
    temp[strcspn(temp, "\n")] = 0;

    size_t line_length = strlen(temp);

    maximum_line_length =
        line_length > maximum_line_length ? line_length : maximum_line_length;

    char **temp_art_array = realloc(art_array, (lines + 1) * sizeof(char *));
    if (temp_art_array == NULL) {
      perror("Failed to allocate memory");
      free_art_array(lines, art_array);
      fclose(fp);
      return NULL;
    }
    art_array = temp_art_array;

    char *temp_string = strdup(temp);
    if (temp_string == NULL) {
      perror("Failed to create string");
      free_art_array(lines, art_array);
      fclose(fp);
      return NULL;
    }
    art_array[lines] = temp_string;

    lines++;
  }

  art = create_art(lines, maximum_line_length, art_array);
  if (art == NULL) {
    perror("Failed to create art");
    free_art_array(lines, art_array);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  return art;
}
