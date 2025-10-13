#ifndef ART_H
#define ART_H

#include <stddef.h>
#include <stdio.h>

struct Art {
  size_t length;
  size_t width;
  char** art_lines;
};

bool CloseArtFile(FILE* art_fp);
void FreeArt(struct Art* art);
FILE* OpenArtFile(const char* art_file_path);
void PrintArt(struct Art* art);
struct Art* ReadArtFile(FILE* art_fp);

#endif
