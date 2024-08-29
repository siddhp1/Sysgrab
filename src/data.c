#include <pwd.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "data.h"

#define DATA_BUFFER_SIZE 128

// Function to remove prefixes, suffixes, and whitespace from a string
char *clean_string (char *string, const char *prefix, const char *suffix)
{
    char *start = string;
    char *end;

    // Remove the prefix if it exists
    if (prefix && strncmp(string, prefix, strlen(prefix)) == 0) {
        start += strlen(prefix);
    }

    // Trim leading whitespace
    while (isspace((unsigned char)*start)) {
        start++;
    }

    // Find the new end after trimming leading whitespace
    end = start + strlen(start);

    // Remove the suffix if it exists
    if (suffix && (end = strstr(start, suffix)) != NULL) {
        *end = '\0';
    }

    // After potentially trimming the suffix, set the `end` pointer again to the last character of the meaningful content.
    end = start + strlen(start) - 1;

    // Trim trailing whitespace
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    // Allocate a new string with the cleaned content
    char *cleaned_string = malloc(strlen(start) + 1);
    if (cleaned_string == NULL) {
        perror("malloc");
        return NULL;
    }

    strcpy(cleaned_string, start);
    return cleaned_string;
}

// Function to extract a double value from a string
double extract_double (const char *string)
{
    // Extract the first double from the input string
    char *endptr;
    double value = strtod(string, &endptr);

    // If no valid conversion could be performed
    if (string == endptr) {
        perror("No valid double found in the string.\n");
        return NAN;
    }

    return value;
}

// Function to convert a double to a string
char *double_to_string(const double value, const char *conv_spec)
{
    // Determine the required buffer size using the custom conversion specifier
    char format[32];
    snprintf(format, sizeof(format), "%%%s", conv_spec);

    // Determine string length of the double
    int size = snprintf(NULL, 0, format, value);
    if (size < 0) {
        perror("snprintf");
        return NULL;
    }

    // Allocate the exact buffer size required
    char *string = malloc(size + 1);  // +1 for the null terminator
    if (string == NULL) {
        perror("malloc");
        return NULL;
    }

    // Convert the result back to a string using the custom conversion specifier
    snprintf(string, size + 1, format, value);
    return string;
}

// Function to extract an integer value from a string
int extract_int(const char *string)
{
    // Extract the first integer from the input string
    char *endptr;

    // Reset errno before calling strtol
    errno = 0;  
    long value = strtol(string, &endptr, 10);

    // If no valid conversion could be performed
    if (string == endptr) {
        perror("No valid integer found in the string.");
        return INT_MIN;
    }

    // Check for overflow and underflow
    if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (value > INT_MAX || value < INT_MIN)) {
        perror("Integer out of range.");
        return INT_MIN;
    }

    return (int)value;
}

char *int_to_string(int value)
{
    // Determine the required buffer size
    int size = snprintf(NULL, 0, "%d", value);
    if (size < 0) {
        perror("snprintf");
        return NULL;
    }

    // Allocate the exact buffer size required
    char *string = malloc(size + 1);  // +1 for the null terminator
    if (string == NULL) {
        perror("malloc");
        return NULL;
    }

    // Convert the result back to a string
    snprintf(string, size + 1, "%d", value);
    return string;
}

// Function to get a string from a command
char *get_from_command (const char *command, const char *look_up, const char *prefix, const char *suffix)
{
    char buffer[DATA_BUFFER_SIZE];
    char *result = malloc(DATA_BUFFER_SIZE);
    
    if (result == NULL) {
        perror("malloc");
        return NULL;
    }

    // Get the output from the command
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error with command: %s\n", command);
        free(result);
        return NULL;
    }

    // Initialize result
    result[0] = '\0';

    // Search for look up value if provided
    if (look_up) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            if (strncmp(buffer, look_up, strlen(look_up)) == 0) {
                strcpy(result, buffer);
                char *cleaned_result = clean_string(result, prefix, suffix);
                free(result);
                result = cleaned_result;
                break;
            }
        }
    } else {
        // Otherwise get first line
        if (fgets(buffer, sizeof(buffer), fp)) {
            strcpy(result, buffer);
            char *cleaned_result = clean_string(result, prefix, suffix);
            free(result);
            result = cleaned_result;
        }
    }

    pclose(fp);
    return result;
}

// Function to get a string from a file
char *get_from_file (const char *file_path, const char *look_up, const char *prefix, const char *suffix)
{
    char buffer[DATA_BUFFER_SIZE];
    char *result = malloc(DATA_BUFFER_SIZE);

    if (result == NULL) {
        perror("malloc");
        return NULL;
    }

    // Read file
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        free(result);
        return NULL;
    }

    // Initialize result
    result[0] = '\0';

    // Search for look up value if provided
    if (look_up) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            if (strncmp(buffer, look_up, strlen(look_up)) == 0) {
                strcpy(result, buffer);
                char *cleaned_result = clean_string(result, prefix, suffix);
                free(result);
                result = cleaned_result;
                break;
            }
        }
    } else {
        // Otherwise get first line
        if (fgets(buffer, sizeof(buffer), fp)) {
            strcpy(result, buffer);
            char *cleaned_result = clean_string(result, prefix, suffix);
            free(result);
            result = cleaned_result;
        }
    }
    fclose(fp);
    return result;
}

// Function to return a formatted string for a system datapoint
char *get_info (DataPoint dp)
{
    char *result = NULL;
    switch (dp) {
        case USERNAME: {
            struct passwd *pw = getpwuid(geteuid());
            if (pw) {
                result = strdup(pw->pw_name);
            } else {
                perror("getpwuid");
            }
            break;
        }
        case HOSTNAME: {
            char hostname[DATA_BUFFER_SIZE];
            if (gethostname(hostname, DATA_BUFFER_SIZE) == 0) {
                result = strdup(hostname);
            } else {
                perror("gethostname");
            }
            break;
        }
        case OS: {
            result = get_from_file("/etc/os-release", "PRETTY_NAME", "PRETTY_NAME=\"", "\"\n");
            break;
        }
        case COMPUTER: {
            char *name = get_from_file("/sys/devices/virtual/dmi/id/product_name", NULL, "", "\n");
            char *version = get_from_file("/sys/devices/virtual/dmi/id/product_version", NULL, "", "\n");
    
            if (name != NULL && version != NULL) {
                // Concatenate name and version
                strcat(name, " ");
                result = strcat(name, version);
            } else {
                result = NULL;
            }
            break;
        }
        case ARCHITECTURE: {
            result = get_from_command("uname -m", NULL, "", "\n");
            break;
        }
        case KERNEL: {
            result = get_from_command("uname -r", NULL, "", "\n");
            break;
        }
        case SHELL: {
            char *shell = getenv("SHELL");

            // Get the token after the last /
            char *last = strrchr(shell, '/');
            if (last != NULL) {
                result = last + 1;
            }
            break;
        }
        case UPTIME: {
            result = get_from_file("/proc/uptime", NULL, "", "\n");
            if (result) {
                // Convert seconds to HH:MM:SS
                int sec = extract_int(result), h, m, s;
                h = sec / 3600;
                m = (sec - (3600 * h)) / 60;
                s = sec - (3600 * h) - (60 * m);
                free(result);

                // Calculate the required buffer size
                int buf_size = snprintf(NULL, 0, "%d:%.2d:%.2d", h, m, s) + 1;
                result = malloc(buf_size);
                if (result) {
                    // Print to a formatted string
                    snprintf(result, buf_size, "%d:%.2d:%.2d", h, m, s);
                }
            }
            break;
        }
        case CPU: {
            char *cpu = get_from_command("lscpu", "Model name:", "Model name:", "\n");
            char *threads = get_from_command("lscpu", "CPU(s):", "CPU(s): ", "\n");
            char *freq = get_from_command("lscpu", "CPU max MHz", "CPU max MHz:", "\n");

            int th = extract_int(threads);
            double gh = extract_double(freq) / 1000;

            if (cpu && threads && freq) {
                char *finalcpu = malloc(strlen(cpu) + strlen(threads) + strlen(freq) + 10);
                if (finalcpu) {
                    // Print to a formatted string
                    snprintf(finalcpu, strlen(cpu) + strlen(threads) + strlen(freq) + 10,
                            "%s (%s) @ %sGHz", cpu, int_to_string(th), double_to_string(gh, ".2f"));
                    free(cpu);
                    free(threads);
                    free(freq);
                    result = finalcpu;
                }
            }
            break;
        }
        case MEMORY: {
            int to = 0, sh = 0, fr = 0, bu = 0, ca = 0, sr = 0;

            char *total_mem = get_from_file("/proc/meminfo", "MemTotal", "MemTotal:", " kB\n");
            if (total_mem) {
                to = extract_int(total_mem);
                free(total_mem);
            }
            char *sh_mem = get_from_file("/proc/meminfo", "Shmem:", "Shmem:", " kB\n");
            if (sh_mem) {
                sh = extract_int(sh_mem);
                free(sh_mem);
            }
            char *free_mem = get_from_file("/proc/meminfo", "MemFree", "MemFree:", " kB\n");
            if (free_mem) {
                fr = extract_int(free_mem);
                free(free_mem);
            }
            char *buffer_mem = get_from_file("/proc/meminfo", "Buffers", "Buffers:", " kB\n");
            if (buffer_mem) {
                bu = extract_int(buffer_mem);
                free(buffer_mem);
            }
            char *sr_mem = get_from_file("/proc/meminfo", "SReclaimable", "SReclaimable:", " kB\n");
            if (sr_mem) {
                sr = extract_int(sr_mem);
                free(sr_mem);
            }
            char *cached_mem = get_from_file("/proc/meminfo", "Cached", "Cached:", " kB\n");
            if (cached_mem) {
                ca = extract_int(cached_mem);
                free(cached_mem);
            }

            // Calculate current memory usage and total available memory in MiB
            char *num = double_to_string((to + sh - fr - bu - ca - sr) / 1024.0, ".0f");
            char *denom = double_to_string(to / 1024.0, ".0f");

            if (num && denom) {
                result = malloc(strlen(num) + strlen(denom) + 20);
                if (result) {
                    // Print to a formatted string
                    snprintf(result, strlen(num) + strlen(denom) + 20, "%sMiB / %sMiB", num, denom);
                }
                free(num);
                free(denom);
            }
            break;
        }
    }
    return result;
}