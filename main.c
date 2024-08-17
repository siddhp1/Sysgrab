#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ART_FILE_NAME "art.txt"

#define CONFIG_FILE_NAME "config.txt"
// add for the config file as well

#define MAX_LEN 40

// Ansi codes (temporary)
#define RESET "\033[0m"
#define GREEN "\033[32m"

typedef struct {
    char *name; // string for the name
    char *value; // string for the value
} Setting;

// fwd declaration for config reading function
void get_config (Setting *config)
{
    FILE *config_fp;

    // Open file and check that it was opened correctly
    config_fp = fopen(CONFIG_FILE_NAME, "r");
    if (config_fp == NULL) {
        printf("Error opening file: %s\n", CONFIG_FILE_NAME);
        return;
    }

    char buffer[MAX_LEN];
    int index = 0;
    while (fgets(buffer, MAX_LEN, config_fp)) {
        char *key = strtok(buffer, "=");
        char *value = strtok(NULL, "\n");

        buffer[strcspn(buffer, "\n")] = 0;

        if (key && value) {
            config[index].name = strdup(key);
            config[index].value = strdup(value);
            index++;
        }
    }

    fclose(config_fp);
}

int main (void) 
{
    // File pointers
    FILE *art_fp;

    Setting settings[2];

    get_config(settings);

    // Open file and check that it was opened correctly
    art_fp = fopen(ART_FILE_NAME, "r");
    if (art_fp == NULL) {
        printf("Error opening file: %s\n", ART_FILE_NAME);
        exit(EXIT_FAILURE);
    }

    int r, g, b;
    sscanf(settings[0].value, "%d,%d,%d", &r, &g, &b);

    // Reading line by line and printing
    char buffer[MAX_LEN];
    while (fgets(buffer, MAX_LEN, art_fp)) {
        // Change the index of the buffer that has the newline character to \0 (removes trailing \n)
        buffer[strcspn(buffer, "\n")] = 0;

        // Printing with colors
        printf(" \033[38;2;%d;%d;%dm%-*s", r, g, b, MAX_LEN+1, buffer);
        printf("siddh@\033[0march \n");
    
    }
    printf("\n");
    
    // Close file
    fclose(art_fp);
    return EXIT_SUCCESS;
}

// config reading function