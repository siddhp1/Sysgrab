#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sysgrab_number.h"

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

unsigned char get_unsigned_char_from_ascii(const char *ascii) {
  if (ascii == NULL) {
    perror("Input string is NULL");
    return 0;
  }

  char *endptr;
  errno = 0;
  long value = strtol(ascii, &endptr, 10);

  if (errno != 0) {
    perror("Failed to get long");
    return 0;
  }

  if (endptr == ascii) {
    perror("No digits were found");
    return 0;
  }

  if (value < 0 || value > UCHAR_MAX) {
    perror("Unsigned char out of range");
    return 0;
  }

  return (unsigned char)value;
}
