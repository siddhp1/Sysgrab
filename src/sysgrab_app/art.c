#include "sysgrab/art.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define MAX_LENGTH 256
#define MAX_WIDTH 256

bool CloseArtFile(FILE* art_fp) {
  if (art_fp == nullptr) {
    log_error("Attempted to close null file pointer");
    return false;
  }

  if (fclose(art_fp) != 0) {
    log_error("Failed to close art file");
    return false;
  }

  return true;
}

void FreeArtLines(char** art_lines, size_t length) {
  if (art_lines == nullptr) return;

  for (size_t line = 0; line < length; line++) free(art_lines[line]);
  free(art_lines);
}

void FreeArt(struct Art* art) {
  if (art == nullptr) return;

  FreeArtLines(art->art_lines, art->length);
  free(art);
}

FILE* OpenArtFile(const char* art_file_path) {
  if (art_file_path == nullptr) {
    log_error("Attempted to open null file path");
    return nullptr;
  }

  FILE* art_fp = fopen(art_file_path, "r");
  if (art_fp == nullptr) {
    log_error("Failed to open art file: %s", art_file_path);
    return nullptr;
  }

  return art_fp;
}

void PrintArt(struct Art* art) {
  if (art == nullptr) {
    log_error("Attempted to print null art");
    return;
  }

  for (size_t line = 0; line < art->length; line++) {
    printf("%s\n", art->art_lines[line]);
  }
}

static size_t TrimLine(char* line_buffer) {
  size_t line_width = strlen(line_buffer);
  while (line_width > 0 && (line_buffer[line_width - 1] == '\n' ||
                            line_buffer[line_width - 1] == '\r' ||
                            line_buffer[line_width - 1] == ' ' ||
                            line_buffer[line_width - 1] == '\t')) {
    line_buffer[--line_width] = '\0';
  }
  return line_width;
}

struct Art* ReadArtFile(FILE* art_fp) {
  if (art_fp == nullptr) {
    log_error("Attempted to read null file pointer");
    return nullptr;
  }

  long start_pos = ftell(art_fp);
  if (start_pos == -1L) {
    log_error("Failed to get art file position");
    return nullptr;
  }

  // First pass: determine art dimensions
  size_t length = 0, width = 0;
  char line_buffer[MAX_WIDTH];
  while (length < MAX_LENGTH &&
         fgets(line_buffer, sizeof(line_buffer), art_fp) != nullptr) {
    size_t line_width = TrimLine(line_buffer);
    if (line_width > width) width = line_width;
    length++;
  }

  if (length == 0) {
    log_error("Art file is empty or unreadable");
    return nullptr;
  }

  if (fseek(art_fp, start_pos, SEEK_SET) != 0) {
    log_error("Failed to reset file position");
    return nullptr;
  }

  char** art_lines = calloc(length, sizeof(char*));
  if (art_lines == nullptr) {
    log_error("Failed to allocate memory for art lines");
    return nullptr;
  }

  // Second pass: read and store art lines
  for (size_t line = 0; line < length; line++) {
    if (fgets(line_buffer, sizeof(line_buffer), art_fp) == nullptr) {
      log_error("Unexpected EOF on second pass");
      FreeArtLines(art_lines, line);
      return nullptr;
    }

    size_t line_width = TrimLine(line_buffer);

    art_lines[line] = malloc((line_width + 1) * sizeof(char));
    if (art_lines[line] == nullptr) {
      log_error("Failed to allocate memory for line %zu", line);
      FreeArtLines(art_lines, line);
      return nullptr;
    }
    strcpy(art_lines[line], line_buffer);
  }

  struct Art* art = malloc(sizeof(struct Art));
  if (art == nullptr) {
    log_error("Failed to allocate memory for art");
    FreeArtLines(art_lines, length);
    return nullptr;
  }

  art->length = length;
  art->width = width;
  art->art_lines = art_lines;

  return art;
}
