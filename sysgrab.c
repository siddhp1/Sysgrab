#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
void print_line (const Color *fg_color, const size_t *max_line_len, char *art_string, char *info_type, char *info_string);
Config *get_config (size_t *config_count);
void free_config (Config *config, size_t config_count);
char **get_art (size_t *line_count, size_t *max_line_len);
void free_art (char **art, size_t line_count);

int main (void) 
{
    // Config editing with args here

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