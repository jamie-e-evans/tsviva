#include "tsviva.h"

#include <stdint.h>
#include <stdlib.h>

/* Add one field to the current row. The array starts at 8 slots and
 * doubles whenever it fills. Returns -1 if it can't get more memory. */
static int push_field(tsviva_reader *r, const char *ptr, size_t len) {
    if (r->n_fields == r->buf_cap_) {
        size_t new_cap = (r->buf_cap_ == 0) ? 8 : r->buf_cap_ * 2;
        if (new_cap > SIZE_MAX / sizeof *r->buf_) {
            return -1;
        }
        tsviva_field *tmp = realloc(r->buf_, new_cap * sizeof *r->buf_);
        if (tmp == NULL) {
            return -1;      /* the old array is still ours; close frees it */
        }
        r->buf_ = tmp;
        r->buf_cap_ = new_cap;
    }

    r->buf_[r->n_fields] = (tsviva_field){.ptr = ptr, .len = len};
    r->n_fields++;
    return 0;
}

static tsviva_status fail(tsviva_reader *r, tsviva_status status,
                          size_t row, size_t offset) {
    r->err.status = status;
    r->err.row = row;
    r->err.offset = offset;
    r->fields = NULL;
    r->n_fields = 0;
    return status;
}

void tsviva_open(tsviva_reader *r, const char *data, size_t len,
                 const tsviva_options *opts) {
    *r = (tsviva_reader){0};
    r->data_ = data;
    r->len_ = len;
    r->allow_ragged_ = (opts != NULL) ? opts->allow_ragged : 0;
}

tsviva_status tsviva_next(tsviva_reader *r) {
    /* After an error, keep returning it. */
    if (r->err.status != TSVIVA_OK) {
        return r->err.status;
    }

    const char *data = r->data_;
    size_t len = r->len_;
    size_t pos = r->pos_;
    size_t row_no = r->row + 1;

    r->fields = NULL;
    r->n_fields = 0;

    /* Nothing left. Also covers trailing newline, so no empty row at end. */
    if (pos >= len) {
        return TSVIVA_DONE;
    }

    for (;;) {
        size_t start = pos;
        while (pos < len && data[pos] != '\t' && data[pos] != '\n' &&
               data[pos] != '\r') {
            pos++;
        }
        if (push_field(r, data + start, pos - start) != 0) {
            return fail(r, TSVIVA_ERR_NOMEM, row_no, 0);
        }

        /* The last row is allowed to have no newline. */
        if (pos == len) {
            break;
        }

        char c = data[pos];
        if (c == '\t') {
            pos++;
            continue;
        }
        if (c == '\n') {
            pos++;
            break;
        }

        /* A CR is only valid as the first half of CR LF. */
        if (pos + 1 < len && data[pos + 1] == '\n') {
            pos += 2;
            break;
        }
        return fail(r, TSVIVA_ERR_BARE_CR, row_no, pos);
    }

    /* The first row sets the width, and later rows have to match it
     * unless the caller allowed ragged rows. */
    size_t n = r->n_fields;
    if (row_no == 1) {
        r->width_ = n;
    } else if (!r->allow_ragged_ && n != r->width_) {
        tsviva_status st = fail(r, TSVIVA_ERR_RAGGED, row_no, 0);
        r->err.expected = r->width_;
        r->err.actual = n;
        return st;
    }

    r->pos_ = pos;
    r->row = row_no;

    /* Set this last: push_field can move buf_ while the row is built. */
    r->fields = r->buf_;
    return TSVIVA_ROW;
}

void tsviva_close(tsviva_reader *r) {
    free(r->buf_);
    r->buf_ = NULL;
    r->buf_cap_ = 0;
    r->fields = NULL;
    r->n_fields = 0;
}
