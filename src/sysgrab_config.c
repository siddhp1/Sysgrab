#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yaml.h>

#include "sysgrab_config.h"
#include "sysgrab_data.h"
#include "sysgrab_number.h"

#define MAX_LEN 256

Config *create_config(bool log_errors, Color base_color, Color accent_color,
                      char *art_file_name, Data *data) {
  Config *config = malloc(sizeof(Config));
  if (config == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  config->log_errors = log_errors;
  config->base_color = base_color;
  config->accent_color = accent_color;
  config->art_file_name = art_file_name;
  config->data = data;

  return config;
}

FILE *open_config_file(const char *file_path) {
  FILE *fp = fopen(file_path, "r");
  if (fp == NULL) {
    fp = fopen(file_path, "w");
    if (fp == NULL) {
      perror("Failed to generate config file");
      return NULL;
    }

    const char *default_yaml = "log_errors: True\n"
                               "art_file_name: art.txt\n"
                               "base_color: [255, 255, 255]\n"
                               "accent_color: [20, 200, 255]\n"
                               "ordering:\n"
                               "  - os\n"
                               "  - architecture\n"
                               "  - kernel\n"
                               "  - computer\n"
                               "  - shell\n"
                               "  - uptime\n"
                               "  - cpu\n"
                               "  - memory\n";
    fwrite(default_yaml, sizeof(char), strlen(default_yaml), fp);
    fclose(fp);

    fp = fopen(file_path, "r");
    if (fp == NULL) {
      perror("Failed to open config file");
      return NULL;
    }
  }
  return fp;
}

Config *get_config(const char *file_path) {
  FILE *fp = open_config_file(file_path);
  if (fp == NULL) {
    perror("Failed to open file");
    return NULL;
  }

  yaml_parser_t parser;
  yaml_event_t event;

  if (!yaml_parser_initialize(&parser)) {
    perror("Failed to initialize parser");
    fclose(fp);
    return NULL;
  }

  yaml_parser_set_input_file(&parser, fp);

  bool log_errors = true;
  Color base_color;
  Color accent_color;
  char *art_file_name = NULL;
  Data *data = create_data();
  if (data == NULL) {
    perror("Failed to create data");
    yaml_parser_delete(&parser);
    fclose(fp);
    return NULL;
  }

  Config *config = NULL;

  char current_key[MAX_LEN] = "";
  bool is_sequence = false;
  size_t sequence_index = 0;

  while (true) {
    if (!yaml_parser_parse(&parser, &event)) {
      fprintf(stderr, "Parser error %d\n", parser.error);
      yaml_event_delete(&event);
      break;
    }
    switch (event.type) {

    case YAML_STREAM_END_EVENT:
      yaml_event_delete(&event);
      goto cleanup;

    case YAML_SCALAR_EVENT: {
      const char *value = (const char *)event.data.scalar.value;

      if (!is_sequence) {
        if (strcmp(current_key, "") == 0) {
          strncpy(current_key, value, sizeof(current_key) - 1);
        } else {
          if (strcmp(current_key, "log_errors") == 0) {
            log_errors = (strcmp(value, "True") == 0);
          } else if (strcmp(current_key, "art_file_name") == 0) {
            art_file_name = strdup(value);
            if (art_file_name == NULL) {
              perror("Failed to create string");
              return NULL;
            }
          }
          strcpy(current_key, "");
        }
      } else {
        if (strcmp(current_key, "ordering") == 0 && strcmp(value, "username") &&
            strcmp(value, "hostname")) {
          DataPoint *datapoint = get_datapoint(value);
          if (datapoint == NULL) {
            perror("Failed to create datapoint");
            return NULL;
          }

          if (!add_datapoint(data, datapoint)) {
            perror("Failed to add datapoint");
            return NULL;
          }
        } else if (strcmp(current_key, "base_color") == 0 ||
                   strcmp(current_key, "accent_color") == 0) {
          Color *color = (strcmp(current_key, "base_color") == 0)
                             ? &base_color
                             : &accent_color;

          if (sequence_index == 0)
            color->red = get_unsigned_char_from_ascii(value);
          else if (sequence_index == 1)
            color->green = get_unsigned_char_from_ascii(value);
          else if (sequence_index == 2)
            color->blue = get_unsigned_char_from_ascii(value);

          sequence_index++;
        }
      }
      break;
    }

    case YAML_SEQUENCE_START_EVENT:
      is_sequence = true;
      sequence_index = 0;
      break;

    case YAML_SEQUENCE_END_EVENT:
      is_sequence = false;
      sequence_index = 0;
      strcpy(current_key, "");
      break;

    default:
      break;
    }

    yaml_event_delete(&event);
  }

cleanup:
  yaml_parser_delete(&parser);
  fclose(fp);

  config =
      create_config(log_errors, base_color, accent_color, art_file_name, data);
  if (config == NULL) {
    perror("Failed to create config");
    return NULL;
  }

  return config;
}
