#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CONFIG_BUFFER_SIZE 64
#define CONFIG_COMMENT_SEQ "//"
#define DEFAULT_BASE_COLOR "255,255,255"
#define DEFAULT_ACCENT_COLOR "0,206,206"

typedef struct {
    char *name;
    char *value;
} Config;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color; 

void generate_config_file (char *config_path);
void edit_config (char *setting, char *value, char *config_path);
Config *get_config (size_t *config_count, char *config_path);
void free_config (Config *config, size_t config_count);

#endif