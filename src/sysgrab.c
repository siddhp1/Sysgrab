#include <getopt.h>
#include <stdbool.h>
#include <libgen.h>

#include "data.h"
#include "config.h"
#include "art.h"

#define VERSION "0.0.1"
#define ART_FILE_PATH "art.txt"
#define CONFIG_FILE_PATH "config.txt"
#define MAX_PATH 1024
#define ERROR_MSG "not found"

void get_executable_path (char *exe_path, size_t size);
void show_help (const char *program_name);
void print_sysgrab (const Color *base_color, const Color *accent_color, char **art, const size_t *max_line_len, const size_t *line_count);
void print_line (const Color *base_color, const Color *accent_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string);

int main (int argc, char *argv[]) 
{
    char exe_path[MAX_PATH];
    char art_path[MAX_PATH];
    char config_path[MAX_PATH];

    // Get the path and directory of the executable
    get_executable_path(exe_path, sizeof(exe_path));
    char *exe_dir = dirname(exe_path);

    // Construct the full paths to the resource files
    snprintf(art_path, sizeof(art_path), "%s/art.txt", exe_dir);
    snprintf(config_path, sizeof(config_path), "%s/config.txt", exe_dir);

    // Check for and generate art/config files if they do not exist
    generate_art_file(art_path);
    generate_config_file(config_path);

    int opt;
    int option_index = 0;

    // Long options
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"base-color", required_argument, 0, 'b'},
        {"accent-color", required_argument, 0, 'a'},
        {0, 0, 0, 0}  // Terminate the array with all zeros
    };

    while ((opt = getopt_long(argc, argv, "hvb:a:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                show_help(argv[0]);
                return EXIT_SUCCESS; 
            case 'v':
                printf("Version %s\n", VERSION);
                return EXIT_SUCCESS; 
            case 'b':
                if (optarg)
                    edit_config("base_color", optarg, config_path);
                else
                    printf("Usage: -b, --base-color [r,g,b]\n");
                break;
            case 'a':
                if (optarg)
                    edit_config("accent_color", optarg, config_path);
                else
                    printf("Usage: -a, --accent-color [r,g,b]\n");
                break;
            case '?':
                fprintf(stderr, "Unknown option: %c\n", optopt);
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);
        }
    }
    
    // Get config and parse
    size_t config_count = 0;
    Config *config = get_config(&config_count, config_path);
    Color base_color, accent_color; 
    if (config != NULL) {
        // Parse specific settings
        for (int i = 0; i < config_count; i++) {
            if (strcmp(config[i].name, "base_color") == 0) {
                sscanf(config[i].value, "%hhu,%hhu,%hhu", &base_color.r, &base_color.g, &base_color.b);
            }
            if (strcmp(config[i].name, "accent_color") == 0) {
                sscanf(config[i].value, "%hhu,%hhu,%hhu", &accent_color.r, &accent_color.g, &accent_color.b);
            }
        }
        free_config(config, config_count);
    }

    // Get art, parse, and print
    size_t max_line_len = 0, line_count = 0; 
    char **art = get_art(&line_count, &max_line_len, art_path);
    if (art != NULL) {
        print_sysgrab(&base_color, &accent_color, art, &max_line_len, &line_count);
        free_art(art, line_count);
    } else {
        print_sysgrab(&base_color, &accent_color, NULL, &max_line_len, NULL);
    }

    return EXIT_SUCCESS;
}

// Function to get executable path
void get_executable_path(char *exe_path, size_t size)
{
    ssize_t len = readlink("/proc/self/exe", exe_path, size - 1);
    if (len != -1) {
        exe_path[len] = '\0';
    } else {
        perror("Error finding executable path");
        exit(EXIT_FAILURE);
    }
}

// Function to print CLI help information
void show_help (const char *program_name)
{
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Modify the configuration settings with the specified options.\n\n");
    printf("Options:\n");
    printf("  (no option)\t\t\tDisplay sysfetch\n");
    printf("  -h, --help\t\t\tShow this help message and exit\n");
    printf("  -v, --version\t\t\tDisplay version information and exit\n");
    printf("  -b, --base-color [r,g,b]\tSet base color in the format r,g,b\n");
    printf("  -a, --accent-color [r,g,b]\tSet accent color in the format r,g,b\n\n");
    printf("Examples:\n");
    printf("  %s\t\t\tDisplay sysfetch\n", program_name);
    printf("  %s -b 255,255,255\tSet base color to white\n", program_name);
    printf("  %s --accent-color 0,0,0\tSet accent color to black\n\n", program_name);
}

void print_sysgrab (const Color *base_color, const Color *accent_color, char **art, const size_t *max_line_len, const size_t *line_count)
{
    char *data_points[] = {
        "OS: ",
        "Architecture: ",
        "Kernel: ",
        "Host: ",
        "Shell: ",
        "Uptime: ",
        "CPU: ",
        "Memory: "
    };

    if (art != NULL) {
        char *username, *hostname;

        username = get_info(USERNAME);
        hostname = get_info(HOSTNAME);
        size_t user_host_len = 0;

        if (username && hostname) {
            user_host_len = strlen(username) + strlen(hostname) + 1;
            strcat(username, "@");

            char dashes[user_host_len + 1];
            memset(dashes, '-', user_host_len);
            dashes[user_host_len] = '\0';

            if (*line_count > 0) {
                print_line(base_color, accent_color, max_line_len, art[0], username, hostname);

            } else {
                print_line(base_color, accent_color, max_line_len, "", username, hostname);
            }

            if (*line_count > 1) {
                print_line(base_color, accent_color, max_line_len, art[1], "", dashes);

            } else {
                print_line(base_color, accent_color, max_line_len, "", "", dashes);
            }

            free(username);
            free(hostname);
        }

        for (DataPoint dp = OS; dp <= MEMORY; dp++) {
            char *info = get_info(dp);
            if (info) {
                if (dp < *line_count) {
                print_line(base_color, accent_color, max_line_len, art[dp], data_points[dp - 2], info); 
                } else {
                    print_line(base_color, accent_color, max_line_len, "", data_points[dp - 2], info);
                }
            } else {
                if (dp < *line_count) {
                print_line(base_color, accent_color, max_line_len, art[dp], data_points[dp - 2], ERROR_MSG); 
                } else {
                    print_line(base_color, accent_color, max_line_len, "", data_points[dp - 2], ERROR_MSG);
                }
            }
        }

        for (int i = 10; i < *line_count; i++) {
            print_line(base_color, accent_color, max_line_len, art[i], "", "");
        }

    } else {
        char *username, *hostname;

        username = get_info(USERNAME);
        hostname = get_info(HOSTNAME);
        size_t user_host_len = 0;

        if (username && hostname) {
            user_host_len = strlen(username) + strlen(hostname) + 1;
            strcat(username, "@");

            char dashes[user_host_len + 1];
            memset(dashes, '-', user_host_len);
            dashes[user_host_len] = '\0';

            print_line(base_color, accent_color, max_line_len, NULL, username, hostname);
            print_line(base_color, accent_color, max_line_len, NULL, "", dashes);

            free(username);
            free(hostname);
        }

        for (DataPoint dp = OS; dp <= MEMORY; dp++) {
            char *info = get_info(dp);
            if (info) {
                print_line(base_color, accent_color, max_line_len, NULL, data_points[dp - 2], info);
            } else {
                print_line(base_color, accent_color, max_line_len, NULL, "", ERROR_MSG);
            }
        }
    }

    printf("\n");
}

// Function to print line
void print_line(const Color *base_color, const Color *accent_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string)
{
    // Change color
    printf("\033[38;2;%d;%d;%dm", accent_color->r, accent_color->g, accent_color->b);

    // Print art (if exists)
    if (art_string != NULL) {
        printf(" %-*s", *max_line_len + 2, art_string);
    }
   
    // Print info type
    printf("%s", info_type);

    // Change color
    printf("\033[38;2;%d;%d;%dm", base_color->r, base_color->g, base_color->b);

    // Print info string
    printf("%s\n", info_string);

    // Reset color
    printf("\033[0m");
}