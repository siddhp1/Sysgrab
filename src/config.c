#include "config.h"

// Function to generate config file with defaults
void generate_config_file (char *config_path)
{
    FILE *cp = fopen(config_path, "r");
    if (cp == NULL) {
        FILE *file = fopen(config_path, "w");
        if (file) {
            fprintf(file, "base_color=%s\n", DEFAULT_BASE_COLOR);
            fprintf(file, "accent_color=%s\n", DEFAULT_ACCENT_COLOR);
            fclose(file);
        } else {
            return;
        }
    } else {
        fclose(cp);
    }
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

// Function to edit the config.txt file
void edit_config (char *setting, char *value, char *config_path)
{
    // Input validation for RGB value
    if (!validate_rgb_value(value)) {
        printf("Invalid value: %s. Expected format: r,g,b with each component between 0 and 255.\n", value);
        return;
    }

    FILE *fp = fopen(config_path, "r+");
    if (fp == NULL) {
        printf("Error opening file: %s\n", config_path);
        return;
    }

    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%s_temp", config_path);

    FILE *temp_fp = fopen(temp_path, "w");
    if (temp_fp == NULL) {
        printf("Error creating temp file: %s\n", temp_path);
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
    if (remove(config_path) != 0) {
        printf("Error deleting original file\n");
        remove(temp_path);
        return;
    }

    if (rename(temp_path, config_path) != 0) {
        printf("Error renaming temp file to original file\n");
        remove(temp_path);
    }
}

// Function to get config key-value pairs from config.txt
Config *get_config (size_t *config_count, char *config_path)
{
    FILE *config_fp = fopen(config_path, "r");
    if (config_fp == NULL) {
        printf("Error opening file: %s\n", config_path);
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

// Function to free dynamically allocated array of config key-value pairs
void free_config(Config *config, size_t config_count)
{
    for (size_t i = 0; i < config_count; i++) {
        free(config[i].name);
        free(config[i].value);
    }
    free(config);
}