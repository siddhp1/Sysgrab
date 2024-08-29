#ifndef ART_H
#define ART_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void generate_art_file (char *art_path);
char **get_art (size_t *line_count, size_t *max_line_len, char *art_path);
void free_art (char **art, size_t line_count);

#endif