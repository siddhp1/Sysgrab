#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "number.h"

double get_double_from_string(const char *string) {
  double value = NAN;

  char *endptr;

  value = strtod(string, &endptr);

  if (string == endptr) {
    perror("Failed to get double");
    return NAN;
  }

  return value;
}

long int get_long_from_string(const char *string) {
  long value = -1;

  char *endptr;

  errno = 0;
  value = strtol(string, &endptr, 10);

  if (string == endptr) {
    perror("Failed to get long");
    return -1;
  }

  if ((errno == ERANGE) && ((value == LONG_MAX || value == LONG_MIN))) {
    perror("Long out of range");
    return -1;
  }

  return value;
}
