#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define VERSION "0.0.1"
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

// Forward declarations
void show_help (const char *program_name);
void edit_config (char *setting, char *value);
void print_line (const Color *fg_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string);
Config *get_config (size_t *config_count);
void free_config (Config *config, size_t config_count);
char **get_art (size_t *line_count, size_t *max_line_len);
void free_art (char **art, size_t line_count);

int main (int argc, char *argv[]) 
{
    // Config editing with args here
    if (argc == 1) {
        // No options provided, perform default action
        printf("Displaying sysfetch...\n");
        // Call your sysfetch function here
        // display_sysfetch();
        EXIT_SUCCESS;
    }

    int opt;
    int option_index = 0;

    // Long options
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"fg-color", required_argument, 0, 'f'},
        {"bg-color", required_argument, 0, 'b'},
        {0, 0, 0, 0}  // Terminate the array with all zeros
    };

    while ((opt = getopt_long(argc, argv, "hvf:b:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                show_help(argv[0]);
                return EXIT_SUCCESS; 
            case 'v':
                printf("Version %s\n", VERSION);
                return EXIT_SUCCESS; 
            case 'f':
                if (optarg)
                    edit_config("fg_color", optarg);
                else
                    printf("Usage: -f, --fg-color [r,g,b]\n");
                break;
            case 'b':
                if (optarg)
                    edit_config("bg_color", optarg);
                else
                    printf("Usage: -b, --bg-color [r,g,b]\n");
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                // show_help(argv[0]);
                exit(EXIT_FAILURE);
            default:
                //show_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    // Config parsing
    size_t config_count = 0;
    Config *config = get_config(&config_count);
    Color fg_color; 
    if (config != NULL) {
        // for (int i = 0; i < config_count; i++) {
        //     printf("%s%s\n", config[i].name, config[i].value);
        // }

        // Parse specific settings
        for (int i = 0; i < config_count; i++) {
            if (strcmp(config[i].name, "fg_color") == 0) {
                sscanf(config[i].value, "%hhu,%hhu,%hhu", &fg_color.r, &fg_color.g, &fg_color.b);
            }
        }

        free_config(config, config_count);
    } else {
        printf("Failed to load configuration.\n");
        return EXIT_FAILURE;
    }

    // Art parsing and printing
    size_t max_line_len = 0; 
    size_t art_count = 0;
    char **art = get_art(&art_count, &max_line_len);
    if (art != NULL) {
        // printf("Maximum line length: %zu\n", max_line_len);
        // printf("Number of lines: %zu\n", art_count);
        
        // printing
        for (int i = 0; i < art_count; i++) {
            print_line(&fg_color, &max_line_len, art[i], "siddh@", "arch");
        }
        printf("\n");

        free_art(art, art_count);
    } else {
        printf("Failed to load art.\n");
        return EXIT_FAILURE;
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
    printf("  -f, --fg-color [r,g,b]\tSet foreground color in the format r,g,b\n");
    printf("  -b, --bg-color [r,g,b]\tSet background color in the format r,g,b\n\n");
    printf("Examples:\n");
    printf("  %s\t\t\tDisplay sysfetch\n", program_name);
    printf("  %s -f 255,255,255\tSet foreground color to white\n", program_name);
    printf("  %s --bg-color 0,0,0\tSet background color to black\n\n", program_name);
}

void edit_config (char *setting, char *value)
{
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

    // Add input validation here if needed
    // format string r,g,b
    // value between 0 and 255

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


void print_line(const Color *fg_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string)
{
    // Change foreground color
    printf("\033[38;2;%d;%d;%dm", fg_color->r, fg_color->g, fg_color->b);
    // Print art and info type
    printf(" %-*s%s", *max_line_len + 2, art_string, info_type);
    // Reset colors
    printf("\033[0m");
    // Print info string
    printf("%s\n", info_string);
}

char **get_art(size_t *line_count, size_t *max_line_len)
{
    FILE *art_fp = fopen(ART_FILE_PATH, "r");
    if (art_fp == NULL) {
        printf("Error opening file: %s\n", ART_FILE_PATH);
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