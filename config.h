#ifndef CONFIG_H
#define CONFIG_H

typedef struct Color {
  char red;
  char green;
  char blue;
} Color;

typedef struct Config {
  bool log_errors;
  Color base_color;
  Color accent_color;
  char *art_file_name; // CHANGE TO ART FILE NAME
  size_t data_functions_count;
  char *(*data_functions)();
} Config;

Config *get_config(const char *file_path);

#endif