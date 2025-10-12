#include <stdio.h>
#include <string.h>

#include "sysgrab_art.h"
#include "sysgrab_config.h"
#include "sysgrab_data.h"
#include "sysgrab_number.h"
#include "sysgrab_print.h"
#include "sysgrab_string.h"

void print_line(Color base_color, Color accent_color, const size_t max_line_len,
                const char *art_string, DataPoint *datapoint,
                const char *other_string) {
  printf("\033[38;2;%d;%d;%dm", accent_color.red, accent_color.green,
         accent_color.blue);
  if (art_string) {
    printf(" %-*s", (int)max_line_len + 2, art_string);
  }
  if (datapoint) {
    printf("%s", datapoint->key);
    printf("\033[38;2;%d;%d;%dm", base_color.red, base_color.green,
           base_color.blue);
    printf("%s", datapoint->value);
  } else if (other_string) {
    printf("\033[38;2;%d;%d;%dm", base_color.red, base_color.green,
           base_color.blue);
    printf("%s", other_string);
  }
  printf("\n\033[0m");
}

void print_sysgrab(const Art *art, const Config *config) {
  if (art == NULL || art->lines == 0) {
    DataPoint *user = get_datapoint("user");
    if (user == NULL) {
      perror("Failed to get datapoint");
      return;
    }

    print_line(config->base_color, config->accent_color, 0, NULL, user, NULL);

    if (config->data->length > 0) {
      size_t length = strlen(user->key) + strlen(user->value);
      if (length <= 0) {
        perror("Length is zero");
        free_datapoint(user);
        return;
      }

      char dashes[length + 1];
      memset(dashes, '-', length);
      dashes[length] = '\0';
      printf("%s\n", dashes);
    }

    free_datapoint(user);

    for (size_t i = 0; i < config->data->length; i++) {
      print_line(config->base_color, config->accent_color, 0, NULL,
                 config->data->datapoints[i], NULL);
    }

    printf("\n");
  } else if (art) {
    DataPoint *user = get_datapoint("user");
    if (user == NULL) {
      perror("Failed to get datapoint");
      return;
    }

    size_t length = strlen(user->key) + strlen(user->value);
    if (length <= 0) {
      perror("Length is zero");
      free_datapoint(user);
      return;
    }

    char dashes[length + 1];
    memset(dashes, '-', length);
    dashes[length] = '\0';

    size_t art_lines = art->lines;
    size_t data_lines = config->data->length > 0 ? config->data->length + 2 : 1;
    size_t total_lines = art_lines > data_lines ? art_lines : data_lines;

    for (size_t i = 0; i < total_lines; i++) {
      char *art_line = NULL;
      char *other_string = NULL;
      DataPoint *datapoint = NULL;

      if (i < art_lines) {
        art_line = art->art_array[i];
      }

      if (i == 0) {
        datapoint = user;
      } else if (i == 1 && config->data->length > 0) {
        other_string = dashes;
      } else if (i - 2 < config->data->length) {
        datapoint = config->data->datapoints[i - 2];
      }

      print_line(config->base_color, config->accent_color,
                 art->maximum_line_length, art_line, datapoint, other_string);
    }

    free_datapoint(user);
    printf("\n");
  }
}
