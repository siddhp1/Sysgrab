#include "art.h"

#define ART_BUFFER_SIZE 1024

// Function to generate empty art file
void generate_art_file (char *art_path)
{
    // Check if file exists
    FILE *ap = fopen(art_path, "r");
    if (ap == NULL) {
        // Create a file
        FILE *file = fopen(art_path, "w");
        if (file) {
            fclose(file);
        } else {
            perror("Failed to generate art file");
            return;
        }
    }
    else {
        fclose(ap);
    }
}

// Function to get ASCII art from art file
char **get_art(size_t *line_count, size_t *max_line_len, char *art_path)
{
    // Read the art file
    FILE *art_fp = fopen(art_path, "r");
    if (art_fp == NULL) {
        return NULL;
    }

    char **art = NULL;
    char buffer[ART_BUFFER_SIZE];
    size_t count = 0;
    *max_line_len = 0;

    while (fgets(buffer, ART_BUFFER_SIZE, art_fp)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        // Get the current line length
        size_t line_len = strlen(buffer);

        // Update the max line length if current line is longer
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
        art[count] = malloc(line_len + 1);
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

    // Set the line count variable
    *line_count = count;
    return art;
}

// Function to free dynamically allocated array art strings
void free_art (char **art, size_t art_count)
{
    // Iterate through the art array and free each string
    for (size_t i = 0; i < art_count; i++) {
        free(art[i]);
    }

    // Free the pointer to the art array
    free(art);
}