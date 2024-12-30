#ifndef ART_H
#define ART_H

typedef struct Art {
  size_t lines;
  size_t maximum_line_length;
  char **art_array;
} Art;

void free_art(Art *art);
Art *get_art(char *file_path);

#endif
