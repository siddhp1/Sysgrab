#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char *name;
    char *value;
} Config;

void generate_config_file (char *config_path);
void edit_config (char *setting, char *value, char *config_path);
Config *get_config (size_t *config_count, char *config_path);
void free_config (Config *config, size_t config_count);

#endif