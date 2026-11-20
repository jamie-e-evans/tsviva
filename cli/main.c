#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INITIAL_CAPACITY 4096

static int read_all(FILE *f, char **out, size_t *out_len) {
    if (!f || !out || !out_len) {
        return EXIT_FAILURE;
    }

    *out = NULL;
    *out_len = 0;

    size_t capacity = INITIAL_CAPACITY;
    char *buffer = malloc(capacity);

    if (!buffer) {
        return EXIT_FAILURE;
    }

    size_t total = 0;

    while (1) {
        if (total == capacity) {
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return EXIT_FAILURE;
            }

            size_t new_capacity = capacity * 2;
            char *tmp = realloc(buffer, new_capacity);

            if (!tmp) {
                free(buffer);
                return EXIT_FAILURE;
            }

            buffer = tmp;
            capacity = new_capacity;
        }

        size_t bytes_read = fread(
            buffer + total,
            1,
            capacity - total,
            f
        );

        total += bytes_read;

        if (bytes_read == 0) {
            if (ferror(f)) {
                free(buffer);
                return EXIT_FAILURE;
            }

            if (feof(f)) {
                break;
            }

            free(buffer);
            return EXIT_FAILURE;
        }
    }

    *out = buffer;
    *out_len = total;

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "rb");

    if (!f) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    char *buffer = NULL;
    size_t buffer_size = 0;

    if (read_all(f, &buffer, &buffer_size) != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to read file\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);

    /* Print as text (not suitable for arbitrary binary data) */
    fwrite(buffer, 1, buffer_size, stdout);
    putchar('\n');

    free(buffer);

    return EXIT_SUCCESS;
}
