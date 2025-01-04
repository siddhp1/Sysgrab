#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 256

char *get_datetime(void) {
  char *datetime = NULL;

  time_t current_time = time(NULL);
  if (current_time == ((time_t)-1)) {
    perror("Failed to get the current time");
    return NULL;
  }

  struct tm *local_time = localtime(&current_time);
  if (local_time == NULL) {
    perror("Failed to convert current time");
    return NULL;
  }

  char temp[MAX_LEN];
  snprintf(temp, sizeof(temp), "%04d-%02d-%02d_%02d-%02d-%02d",
           local_time->tm_year + 1900, local_time->tm_mon + 1,
           local_time->tm_mday, local_time->tm_hour, local_time->tm_min,
           local_time->tm_sec);

  datetime = strdup(temp);

  return datetime;
}
