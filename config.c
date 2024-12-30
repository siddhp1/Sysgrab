#include <stdbool.h>
#include <stdio.h>

#include <yaml.h>

#include "config.h"

#define MAX_LEN 256
// Defaults here

Config *create_config(bool log_errors, Color base_color, Color accent_color,
                      char *art_file_name, size_t data_functions_count,
                      char *(*data_functions)()) {
  Config *config = malloc(sizeof(Config));
  if (config == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  config->log_errors = log_errors;
  config->base_color = base_color;
  config->accent_color = accent_color;
  config->art_file_name = art_file_name;
  config->data_functions = data_functions;
  config->data_functions_count = data_functions_count;

  return config;
}

void free_config(Config *config) {
  free(config->data_functions);
  free(config);
}

// Turn this into an open file function
bool generate_config_file(char *file_path) {}

Config *get_config(const char *file_path) {
  yaml_parser_t parser;
  yaml_event_t event;

  Color base_color = {};
  Color accent_color = {};

  Config *config = NULL;

  FILE *fp = fopen(file_path, "r");
  if (fp == NULL) {
    perror("Failed to open file");
    return NULL;
  }

  if (!yaml_parser_initialize(&parser)) {
    perror("Failed to initialize parser");
    fclose(fp);
    return NULL;
  }

  yaml_parser_set_input_file(&parser, fp);

  char key[MAX_LEN] = {0}; // See if this needs to be initalized
  bool is_sequence = false;
  size_t sequence_index = 0;

  while (true) {
    if (!yaml_parser_parse(&parser, &event)) {
      fprintf(stderr, "Parser error %d\n", parser.error);
      break;
    }

    // Remove other cases that havr nothign after
    // Remove logging and stuff too
    switch (event.type) {

    // This one is not required
    case YAML_STREAM_START_EVENT:
      puts("Stream start");
      break;
      // End delete here

    case YAML_STREAM_END_EVENT:
      puts("Stream end");
      goto cleanup;
    case YAML_SCALAR_EVENT:
      const char *value = (const char *)event.data.scalar.value;
      puts(value); // Remove after

      if (is_sequence) {
        if ((strcmp(key, "base_color") == 0) ||
            (strcmp(key, "accent_color") == 0)) {
          // Make the rbg thing
        }

      } else {
      }

      // Process scalar data and populate the `Config` structure
      break;
    case YAML_MAPPING_START_EVENT:
      puts("Mapping start");
      break;
    case YAML_MAPPING_END_EVENT:
      puts("Mapping end");
      break;
    case YAML_SEQUENCE_START_EVENT:
      puts("Sequence start");
      break;
    case YAML_SEQUENCE_END_EVENT:
      puts("Sequence end");
      break;
    default:
      break;
    }

    yaml_event_delete(&event);
  }

cleanup:
  yaml_parser_delete(&parser);
  fclose(fp);

  return config;
}

int main(void) {
  get_config("config.yaml");
  return 0;
}
