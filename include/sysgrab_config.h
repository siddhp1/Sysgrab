#ifndef SYSGRAB_CONFIG_H
#define SYSGRAB_CONFIG_H

#include "sysgrab_data.h"

typedef struct Color {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} Color;

typedef struct Config {
  bool log_errors;
  Color base_color;
  Color accent_color;
  char *art_file_name;
  Data *data;
} Config;

Config *get_config(const char *file_path);

#endif
