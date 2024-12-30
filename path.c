#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEN 1024

char *get_executable_directory_path(void) {
  char *path = NULL;

  char temp[MAX_LEN];
  ssize_t length = readlink("/proc/self/exe", temp, sizeof(temp) - 1);

  if (length >= 0) {
    temp[length] = '\0';
    path = strdup(dirname(temp));
  } else {
    perror("Failed to get executable path");
    return NULL;
  }

  return path;
}

char *get_file_path(const char *file_name) {
  char *path = NULL;

  char *executable_directory_path = get_executable_directory_path();
  if (executable_directory_path == NULL) {
    perror("Failed to get exectable directory path");
    return NULL;
  }

  char temp[MAX_LEN];
  snprintf(temp, sizeof(temp), "%s/%s", executable_directory_path, file_name);

  path = strdup(temp);

  free(executable_directory_path);
  return (path);
}
