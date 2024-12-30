#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

#define MAX_LEN 256

char *clean_string(const char *original_string, const char *prefix,
                   const char *suffix) {
  char *temp = strdup(original_string);
  if (temp == NULL) {
    perror("Failed to create string");
    return NULL;
  }

  char *start = temp;
  char *end = NULL;
  char *clean_string = NULL;

  if (prefix && strncmp(original_string, prefix, strlen(prefix)) == 0) {
    start += strlen(prefix);
  }

  // Trim leading whitespace
  while (isspace((unsigned char)*start)) {
    start++;
  }

  end = start + strlen(start);

  if (suffix && (end = strstr(start, suffix)) != NULL) {
    *end = '\0';
  }

  end = start + strlen(start) - 1;

  // Trim trailing whitespace
  while (end > start && isspace((unsigned char)*end)) {
    *end = '\0';
    end--;
  }

  clean_string = strdup(start);
  if (clean_string == NULL) {
    perror("Failed to create string");
    return NULL;
  }

  free(temp);
  return clean_string;
}

char *get_string_from_command(const char *command, const char *look_up) {
  char temp[MAX_LEN];

  char *string = NULL;

  if (command == NULL) {
    perror("Invalid command");
    return NULL;
  }

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    perror("Failed to run command");
    return NULL;
  }

  if (look_up) {
    while (fgets(temp, sizeof(temp), fp)) {
      if (strncmp(temp, look_up, strlen(look_up)) == 0) {
        string = strdup(temp);
        if (string == NULL) {
          perror("Failed to create string");
          return NULL;
        }

        goto success;
      }
    }
    perror("Failed to find look up");
  } else if (fgets(temp, sizeof(temp), fp)) {
    string = strdup(temp);
    if (string == NULL) {
      perror("Failed to create string");
      return NULL;
    }
  } else {
    perror("Failed to get string from command");
  }

success:
  pclose(fp);
  return string;
}

char *get_string_from_double(const double value,
                             const char *conversion_specifier) {
  char *string = NULL;

  // Determine required buffer size
  char format[MAX_LEN];
  snprintf(format, sizeof(format), "%%%s", conversion_specifier);

  int length = snprintf(NULL, 0, format, value);
  if (length <= 0) {
    perror("Length is zero");
    return NULL;
  }

  string = malloc((length + 1) * sizeof(char));
  if (string == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  snprintf(string, length + 1, format, value);

  return string;
}

char *get_string_from_file(const char *file_path, const char *look_up) {
  char temp[MAX_LEN];

  char *string = NULL;

  if (file_path == NULL) {
    perror("Invalid file path");
    return NULL;
  }

  FILE *fp = fopen(file_path, "r");
  if (fp == NULL) {
    perror("Failed to open file");
    return NULL;
  }

  if (look_up) {
    while (fgets(temp, sizeof(temp), fp)) {
      if (strncmp(temp, look_up, strlen(look_up)) == 0) {
        string = strdup(temp);
        if (string == NULL) {
          perror("Failed to create string");
          return NULL;
        }

        goto success;
      }
    }
    perror("Failed to find look up");
  } else if (fgets(temp, sizeof(temp), fp)) {
    string = strdup(temp);
    if (string == NULL) {
      perror("Failed to create string");
      return NULL;
    }
  } else {
    perror("Failed to get string from file");
  }

success:
  fclose(fp);
  return string;
}

char *get_string_from_long(const long value) {
  char *string = NULL;

  int length = snprintf(NULL, 0, "%ld", value);
  if (length <= 0) {
    perror("Length is zero");
    return NULL;
  }

  string = malloc((length + 1) * sizeof(char));
  if (string == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  snprintf(string, length + 1, "%ld", value);

  return string;
}
