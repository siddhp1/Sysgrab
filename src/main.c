#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "sysgrab_art.h"
#include "sysgrab_config.h"
#include "sysgrab_log.h"
#include "sysgrab_path.h"
#include "sysgrab_print.h"

#define CONFIG_FILE_NAME "config.yaml"
#define REPO_URL "github.com/siddhp1/Sysgrab"
#define VERSION "2.0.0"

int main(int argc, char *argv[]) {
  int opt;
  int option_index = 0;

  static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                         {"version", no_argument, 0, 'v'},
                                         {"delete-logs", no_argument, 0, 'd'},
                                         {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "hvd", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      printf("Documentation can be found at %s\n", REPO_URL);
      exit(EXIT_SUCCESS);
    case 'v':
      printf("Version %s\n", VERSION);
      exit(EXIT_SUCCESS);
    case 'd':
      if (!delete_logs()) {
        printf("Failed to delete logs\n");
      } else {
        printf("Successfully deleted logs\n");
      }
      exit(EXIT_SUCCESS);
    case '?':
      if (optopt) {
        fprintf(stderr, "Invalid option: -%c\n", optopt);
      } else {
        fprintf(stderr, "Invalid option: %s\n", argv[optind - 1]);
      }
    default:
      exit(EXIT_FAILURE);
    }
  }

  const char *log_file_path = get_log_file_path();

  FILE *log_fp = NULL;
  log_fp = freopen(log_file_path, "w", stderr);
  if (log_fp == NULL) {
    perror("Failed to redirect stderr");
    exit(EXIT_FAILURE);
  }

  char *config_path = get_file_path(CONFIG_FILE_NAME);
  if (config_path == NULL) {
    perror("Failed to get config file path");
    exit(EXIT_FAILURE);
  }

  Config *config = get_config(config_path);
  if (config == NULL) {
    perror("Failed to get config");
    exit(EXIT_FAILURE);
  }

  char *art_path = get_file_path(config->art_file_name);

  Art *art = get_art(art_path);

  print_sysgrab(art, config);

  free(config->data);
  free(config);

  free_art(art);

  free(config_path);
  free(art_path);

  fclose(log_fp);

  exit(EXIT_SUCCESS);
}
