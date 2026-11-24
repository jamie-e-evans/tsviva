#ifndef TSVIVA_H
#define TSVIVA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A field: a slice of the caller's input buffer. Not NUL-terminated, and may
 * contain any byte except TAB, LF, and CR. */
typedef struct {
    const char *ptr;
    size_t len;
} tsviva_field;

typedef enum {
    TSVIVA_OK = 0,      /* no error, the starting state */
    TSVIVA_ROW,         /* tsviva_next: a row is ready */
    TSVIVA_DONE,        /* tsviva_next: end of input, no more rows */
    TSVIVA_ERR_BARE_CR,
    TSVIVA_ERR_RAGGED,
    TSVIVA_ERR_NOMEM
} tsviva_status;

/* Details of the most recent error. See SPEC.md, Errors */
typedef struct {
    tsviva_status status;
    size_t row;         /* 1-based */
    size_t offset;      /* byte offset; set for TSVIVA_ERR_BARE_CR */
    size_t expected;    /* set for TSVIVA_ERR_RAGGED */
    size_t actual;      /* set for TSVIVA_ERR_RAGGED */
} tsviva_error;

typedef struct {
    int allow_ragged;   /* 0 (the default) = enforce width check */
} tsviva_options;


/* Create with tsviva_open and release with tsviva_close. Do not copy a
 * reader: copies would share one field array. */
typedef struct {
    /* Public, read-only: valid until next call to tsviva_next. */
    const tsviva_field *fields;
    size_t n_fields;
    size_t row;         /* 1-based number of the current row */
    tsviva_error err;

    /* Private: do not read or write */
    const char *data_;
    size_t len_;
    size_t pos_;
    tsviva_field *buf_;
    size_t buf_cap_;
    size_t width_;
    int allow_ragged_;
} tsviva_reader;

/* Starts reading the len bytes at data. The reader borrows data: it must
 * stay valid and unmodified until tsviva_close. opts is read only during
 * this call and may be NULL for the defaults. Allocates nothing. */
void tsviva_open(tsviva_reader *r, const char *data, size_t len,
                 const tsviva_options *opts);

/* Advances to the next row. Returns TSVIVA_ROW when r->fields holds it,
 * TSVIVA_DONE at the end of the input, or an error status (details in
 * r->err). After an error or DONE, further calls return the same result. The
 * fields are valid only until the next call. */
tsviva_status tsviva_next(tsviva_reader *r);

/* Release the reader's memory. Safe to call after any result. */
void tsviva_close(tsviva_reader *r);

#ifdef __cplusplus
}
#endif

#endif /* TSVIVA_H */
