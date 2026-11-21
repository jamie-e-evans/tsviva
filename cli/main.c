#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 4096

/* Reads f to the end into *buf, which must point to a malloc'd block of
   *cap bytes. Grows the block with realloc when needed, updating *buf and
   *cap. Stores the byte count in *out_len. Returns 0 on success, -1 on
   failure. On failure *buf is still a valid block that the caller must
   free. */
static int read_all(FILE *f, char **buf, size_t *cap, size_t *out_len) {
    size_t total = 0;

    for (;;) {
        if (total == *cap) {
            if (*cap > SIZE_MAX / 2) {
                return -1;
            }
            char *tmp = realloc(*buf, *cap * 2);
            if (tmp == NULL) {
                return -1;          /* *buf is untouched and still valid */
            }
            *buf = tmp;
            *cap *= 2;
        }

        size_t n = fread(*buf + total, 1, *cap - total, f);
        total += n;

        if (n == 0) {
            if (ferror(f)) {
                return -1;
            }
            break;                  /* end of file */
        }
    }

    *out_len = total;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    size_t capacity = INITIAL_CAPACITY;
    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        fclose(f);
        fprintf(stderr, "out of memory\n");
        return EXIT_FAILURE;
    }

    size_t len = 0;
    int rc = read_all(f, &buffer, &capacity, &len);
    fclose(f);
    if (rc != 0) {
        free(buffer);
        fprintf(stderr, "%s: read failed\n", argv[1]);
        return EXIT_FAILURE;
    }

    fwrite(buffer, 1, len, stdout);
    free(buffer);
    return EXIT_SUCCESS;
}
