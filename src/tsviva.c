#include "tsviva.h"

#include <stdlib.h>

void tsviva_open(tsviva_reader *r, const char *data, size_t len,
                 const tsviva_options *opts) {
    *r = (tsviva_reader){0};
    r->data_ = data;
    r->len_ = len;
    r->allow_ragged_ = (opts != NULL) ? opts->allow_ragged : 0;
}

tsviva_status tsviva_next(tsviva_reader *r) {
    (void)r;            /* unused until the parser exists */
    return TSVIVA_DONE;
}

void tsviva_close(tsviva_reader *r) {
    free(r->buf_);
    r->buf_ = NULL;
    r->buf_cap_ = 0;
    r->fields = NULL;
    r->n_fields = 0;
}
