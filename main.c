#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// My headers here
#include "path.h"

// CLI and file names here
#define CONFIG_FILE_NAME "config.yaml"

// #define ERROR_FILE_NAME "error.log" REMOVE THIS AFTER

#define REPO_URL "github.com/siddhp1/Sysgrab"
#define VERSION "2.0.0"

int main(int argc, char *argv[]) {
  int opt;
  int option_index = 0;

  static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                         {"version", no_argument, 0, 'v'},
                                         {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "hv", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      printf("Visit %s for help\n", REPO_URL);
      exit(EXIT_SUCCESS);
    case 'v':
      printf("Version %s\n", VERSION);
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

  char *config_path = get_file_path(CONFIG_FILE_NAME);
  if (config_path == NULL) {
    perror("Failed to get config file path");
    exit(EXIT_FAILURE);
  }

  // PARSE CONFIG
  // HERE

  // DO THIS CONDITIONALLY BASED ON THE CONFIG
  // IF WE REQUEST LOG FILES, GENERATE THE PATH WITH THE FILEPATH FUNCTION AND
  // USE THE CURRENT DATE TIME
  FILE *error_fp = freopen("error.log", "w", stderr);
  if (error_fp == NULL) {
    perror("Failed to redirect stderr");
    exit(EXIT_FAILURE);
  }

  // GET THE ART
  // HERE

  // CALL THE PRINT FUNCTION
  // DEFINE IN PRINT.C / .H

  free(config_path);
  // free(art_path);

  fclose(error_fp);

  exit(EXIT_SUCCESS);
}