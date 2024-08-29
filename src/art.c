#include "art.h"

// Function to generate empty art file
void generate_art_file (char *art_path)
{
    FILE *ap = fopen(art_path, "r");
    if (ap == NULL) {
        FILE *file = fopen(art_path, "w");
        if (file) {
            fclose(file);
        } else {
            return;
        }
    }
    else {
        fclose(ap);
    }
}

char **get_art(size_t *line_count, size_t *max_line_len, char *art_path)
{
    FILE *art_fp = fopen(art_path, "r");
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