#include "config.h"

#define CONFIG_BUFFER_SIZE 64
#define TEMP_PATH_LENGTH 256
#define CONFIG_COMMENT_SEQ "//"
#define DEFAULT_BASE_COLOR "255,255,255"
#define DEFAULT_ACCENT_COLOR "20,200,255"

// Function to generate config file with defaults
void generate_config_file (char *config_path)
{
    // Check if file exists
    FILE *cp = fopen(config_path, "r");
    if (cp == NULL) {
        FILE *file = fopen(config_path, "w");
        if (file) {
            // Write default strings to the new file
            fprintf(file, "base_color=%s\n", DEFAULT_BASE_COLOR);
            fprintf(file, "accent_color=%s\n", DEFAULT_ACCENT_COLOR);
            fclose(file);
        } else {
            perror("Failed to generate config file");
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
    
    // Check if numbers are between 0 and 255
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
        fprintf(stderr, "Invalid value: %s. Expected format: r,g,b with each component between 0 and 255.\n", value);
        return;
    }

    // Open file to read and write
    FILE *fp = fopen(config_path, "r+");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", config_path);
        return;
    }

    // Generate a temporary file path
    char temp_path[TEMP_PATH_LENGTH];
    snprintf(temp_path, sizeof(temp_path), "%s_temp", config_path);

    // Create a new file
    FILE *temp_fp = fopen(temp_path, "w");
    if (temp_fp == NULL) {
        fprintf(stderr, "Error creating temp file: %s\n", temp_path);
        fclose(fp);
        return;
    }

    // Read the old config file and add to the new file with updated values
    char line[CONFIG_BUFFER_SIZE];
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

    // If setting not found in the old file, append it at the end of the new file
    if (!found) {
        fprintf(temp_fp, "%s=%s\n", setting, value);
    }

    fclose(fp);
    fclose(temp_fp);

    // Remove the original file
    if (remove(config_path) != 0) {
        perror("Error deleting original file");
        remove(temp_path);
        return;
    }

    // Rename the new file to the original name
    if (rename(temp_path, config_path) != 0) {
        perror("Error renaming temp file to original file");
        remove(temp_path);
    }
}

// Function to get config key-value pairs from config file
Config *get_config (size_t *config_count, char *config_path)
{
    // Read the config file
    FILE *config_fp = fopen(config_path, "r");
    if (config_fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", config_path);
        return NULL;
    }

    Config *config = NULL;
    char buffer[CONFIG_BUFFER_SIZE];
    size_t count = 0;

    while (fgets(buffer, CONFIG_BUFFER_SIZE, config_fp)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0; 

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

            // Set the key and value
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

    // Set the config count variable
    *config_count = count;
    
    return config;
}

// Function to free dynamically allocated array of config key-value pairs
void free_config(Config *config, size_t config_count)
{
    // Iterate through the config and free each key and value
    for (size_t i = 0; i < config_count; i++) {
        free(config[i].name);
        free(config[i].value);
    }
    
    // Free the pointer to the config array
    free(config);
}