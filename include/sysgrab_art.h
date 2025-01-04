#ifndef SYSGRAB_ART_H
#define SYSGRAB_ART_H

#include <stddef.h>

typedef struct Art {
  size_t lines;
  size_t maximum_line_length;
  char **art_array;
} Art;

void free_art(Art *art);
Art *get_art(const char *file_path);

#endif
