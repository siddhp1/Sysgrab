#ifndef SYSGRAB_STRING_H
#define SYSGRAB_STRING_H

char *clean_string(const char *original_string, const char *prefix,
                   const char *suffix);
char *get_string_from_command(const char *command, const char *look_up);
char *get_string_from_double(const double value,
                             const char *conversion_specifier);
char *get_string_from_file(const char *file_path, const char *look_up);
char *get_string_from_long(const long value);

#endif
