#include <getopt.h>
#include <stdbool.h>
#include "data.h"

#define VERSION "0.0.1"
#define ERROR_MSG "not found"
#define ART_FILE_PATH "art.txt"
#define CONFIG_FILE_PATH "config.txt"
#define CONFIG_BUFFER_SIZE 64
#define ART_BUFFER_SIZE 1024
#define CONFIG_COMMENT_SEQ "//"

typedef struct {
    char *name;
    char *value;
} Config;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color; 

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

// Forward declarations
void show_help (const char *program_name);
void edit_config (char *setting, char *value);
void print_line (const Color *base_color, const Color *accent_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string);
Config *get_config (size_t *config_count);
void free_config (Config *config, size_t config_count);
char **get_art (size_t *line_count, size_t *max_line_len);
void free_art (char **art, size_t line_count);

int main (int argc, char *argv[]) 
{
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
                    edit_config("base_color", optarg);
                else
                    printf("Usage: -b, --base-color [r,g,b]\n");
                break;
            case 'a':
                if (optarg)
                    edit_config("accent_color", optarg);
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
    
    // Config parsing
    size_t config_count = 0;
    Config *config = get_config(&config_count);
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

    // Default values for settings if not found in config file
    if (!base_color.r || !base_color.g || !base_color.b) {
        base_color.r = 255;
        base_color.g = 255; 
        base_color.b = 255; 
    }
    if (!accent_color.r || !accent_color.g || !accent_color.b) {
        accent_color.r = 64; 
        accent_color.g = 224; 
        accent_color.b = 208; 
    }

    // Art parsing and printing
    size_t max_line_len = 0, line_count = 0; 
    char **art = get_art(&line_count, &max_line_len);

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

            if (line_count > 0) {
                print_line(&base_color, &accent_color, &max_line_len, art[0], username, hostname);

            } else {
                print_line(&base_color, &accent_color, &max_line_len, "", username, hostname);
            }

            if (line_count > 1) {
                print_line(&base_color, &accent_color, &max_line_len, art[1], "", dashes);

            } else {
                print_line(&base_color, &accent_color, &max_line_len, "", "", dashes);
            }

            free(username);
            free(hostname);
        }

        for (DataPoint dp = OS; dp <= MEMORY; dp++) {
            char *info = get_info(dp);
            if (info) {
                if (dp < line_count) {
                print_line(&base_color, &accent_color, &max_line_len, art[dp], data_points[dp - 2], info); 
                } else {
                    print_line(&base_color, &accent_color, &max_line_len, "", data_points[dp - 2], info);
                }
            } else {
                if (dp < line_count) {
                print_line(&base_color, &accent_color, &max_line_len, art[dp], data_points[dp - 2], ERROR_MSG); 
                } else {
                    print_line(&base_color, &accent_color, &max_line_len, "", data_points[dp - 2], ERROR_MSG);
                }
            }
        }

        for (int i = 10; i < line_count; i++) {
            print_line(&base_color, &accent_color, &max_line_len, art[i], "", "");
        }

        printf("\n");
        free_art(art, line_count);
    } else {
        char *username, *hostname;

        username = get_info(USERNAME);
        hostname = get_info(HOSTNAME);
        size_t user_host_len = 0;

        if (username && hostname) {
            user_host_len = strlen(username) + strlen(hostname) + 1;
            strcat(username, "@");
            print_line(&base_color, &accent_color, &max_line_len, NULL, username, hostname);
            free(username);
            free(hostname);
        }

        char dashes[user_host_len + 1];
        memset(dashes, '-', user_host_len);
        dashes[user_host_len] = '\0';

        print_line(&base_color, &accent_color, &max_line_len, NULL, "", dashes);

        for (DataPoint dp = OS; dp <= MEMORY; dp++) {
            char *info = get_info(dp);
            if (info) {
                print_line(&base_color, &accent_color, &max_line_len, NULL, data_points[dp - 2], info);
                free(info);
            } else {
                print_line(&base_color, &accent_color, &max_line_len, NULL, "", ERROR_MSG);
                free(info);
            }
        }

        printf("\n");
    }

    return EXIT_SUCCESS;
}

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

// Function to validate the format of r,g,b values
bool validate_rgb_value(const char *value) {
    int r, g, b;
    char temp;
    
    // Check if the value is in the format r,g,b and each component is within the 0-255 range
    if (sscanf(value, "%d,%d,%d%c", &r, &g, &b, &temp) != 3) {
        return false;
    }
    
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        return false;
    }
    
    return true;
}

void edit_config (char *setting, char *value)
{
    // Input validation for RGB value
    if (!validate_rgb_value(value)) {
        printf("Invalid value: %s. Expected format: r,g,b with each component between 0 and 255.\n", value);
        return;
    }

    FILE *fp = fopen(CONFIG_FILE_PATH, "r+");
    if (fp == NULL) {
        printf("Error opening file: %s\n", CONFIG_FILE_PATH);
        return;
    }

    char temp_filename[] = "config_tempXXXXXX";
    int temp_fd = mkstemp(temp_filename);
    if (temp_fd == -1) {
        printf("Error creating temp file\n");
        fclose(fp);
        return;
    }

    FILE *temp_fp = fdopen(temp_fd, "w");
    if (temp_fp == NULL) {
        printf("Error opening temp file stream\n");
        close(temp_fd);
        fclose(fp);
        return;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        // Check if the line starts with the setting we want to edit
        if (strncmp(line, setting, strlen(setting)) == 0 && line[strlen(setting)] == '=') {
            fprintf(temp_fp, "%s=%s\n", setting, value);
            found = 1;
        } else {
            fputs(line, temp_fp);
        }
    }

    if (!found) {
        // If setting not found, append it at the end of the file
        fprintf(temp_fp, "%s=%s\n", setting, value);
    }

    fclose(fp);
    fclose(temp_fp);

    // Replace the original file with the temp file
    if (remove(CONFIG_FILE_PATH) != 0) {
        printf("Error deleting original file\n");
        return;
    }

    if (rename(temp_filename, CONFIG_FILE_PATH) != 0) {
        printf("Error renaming temp file to original file\n");
    }
}

void print_line(const Color *base_color, const Color *accent_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string)
{
    // Change color
    printf("\033[38;2;%d;%d;%dm", accent_color->r, accent_color->g, accent_color->b);

    // Print art
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

char **get_art(size_t *line_count, size_t *max_line_len)
{
    FILE *art_fp = fopen(ART_FILE_PATH, "r");
    if (art_fp == NULL) {
        return NULL;
    }

    char **art = NULL;
    char buffer[ART_BUFFER_SIZE];
    size_t count = 0;
    *max_line_len = 0;

    while (fgets(buffer, ART_BUFFER_SIZE, art_fp)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        size_t line_len = strlen(buffer);
        
        // Update the max line length if this line is longer
        if (line_len > *max_line_len) {
            *max_line_len = line_len;
        }

        // Allocate space for the new line in the array of pointers
        char **new_art = realloc(art, (count + 1) * sizeof(char *));
        if (new_art == NULL) {
            perror("realloc");
            for (size_t i = 0; i < count; i++) {
                free(art[i]);
            }
            free(art);
            fclose(art_fp);
            return NULL;
        }
        art = new_art;

        // Allocate exact space for the line
        art[count] = malloc(line_len + 1); // +1 for the null terminator
        if (art[count] == NULL) {
            perror("malloc");
            for (size_t i = 0; i < count; i++) {
                free(art[i]);
            }
            free(art);
            fclose(art_fp);
            return NULL;
        }

        // Copy the line into the allocated memory
        strcpy(art[count], buffer);

        count++;
    }

    fclose(art_fp);

    *line_count = count;
    return art;
}

void free_art (char **art, size_t art_count)
{
    for (size_t i = 0; i < art_count; i++) {
        free(art[i]);
    }
    free(art);
}

Config *get_config (size_t *config_count)
{
    FILE *config_fp = fopen(CONFIG_FILE_PATH, "r");
    if (config_fp == NULL) {
        printf("Error opening file: %s\n", CONFIG_FILE_PATH);
        return NULL;
    }

    Config *config = NULL;
    char buffer[CONFIG_BUFFER_SIZE];
    size_t count = 0;

    while (fgets(buffer, CONFIG_BUFFER_SIZE, config_fp)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        // Ignore comment lines
        if (strncmp(buffer, CONFIG_COMMENT_SEQ, 2) == 0) {
            continue;
        }

        // Tokenize the line
        char *key = strtok(buffer, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            // Allocate space for the new config
            Config *new_config = realloc(config, (count + 1) * sizeof(Config));
            if (new_config == NULL) {
                perror("realloc");
                free(config);
                fclose(config_fp);
                return NULL;
            }
            config = new_config;

            config[count].name = strdup(key);
            config[count].value = strdup(value);

            if (config[count].name == NULL || config[count].value == NULL) {
                perror("strdup");
                free(config);
                fclose(config_fp);
                return NULL;
            }

            count++;
        }
    }

    fclose(config_fp);
    *config_count = count;
    return config;
}

void free_config(Config *config, size_t config_count)
{
    for (size_t i = 0; i < config_count; i++) {
        free(config[i].name);
        free(config[i].value);
    }
    free(config);
}