#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sysgrab_path.h"
#include "sysgrab_time.h"

#define LOG_FILE_DIRECTORY "logs"
#define MAX_LEN 1024

char *get_log_file_directory_path(void) {
  char *path = get_file_path(LOG_FILE_DIRECTORY);
  if (path == NULL) {
    perror("Failed to get exectable directory path");
    return NULL;
  }

  if (access(path, F_OK) == -1) {
    if (mkdir(path, 0755) == -1) {
      perror("Failed to create directory");
      return NULL;
    }
  }

  return path;
}

char *get_log_file_path(void) {
  char *path = NULL;

  char *log_file_directory_path = get_log_file_directory_path();
  if (log_file_directory_path == NULL) {
    perror("Failed to get log file directory path");
    return NULL;
  }

  char *datetime = get_datetime();

  char temp[MAX_LEN];
  snprintf(temp, sizeof(temp), "%s/%s.log", log_file_directory_path, datetime);

  path = strdup(temp);

  free(datetime);

  return path;
}

bool delete_logs(void) {
  char *log_file_directory_path = get_log_file_directory_path();
  if (log_file_directory_path == NULL) {
    perror("Failed to get log file directory path");
    return false;
  }

  char temp[1024];
  snprintf(temp, sizeof(temp), "rm -rf \"%s\"", log_file_directory_path);

  int status = system(temp);
  if (status != 0) {
    perror("Failed to delete logs");
    return false;
  }

  free(log_file_directory_path);

  return true;
}
