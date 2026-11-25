#!/bin/sh
# Generates tests/fixtures/. \t is TAB, \r is CR. Bytes are written in octal to
# generate fixtures correctly with any POSIX printf.
# Run from the project root: sh tests/make_fixtures.sh
# Each fixture is the smallest input that tests one rule in docs/SPEC.md.
set -eu

v=tests/fixtures/valid
i=tests/fixtures/invalid
mkdir -p "$v" "$i"

# valid input ------------------------------------------------------------------
: > "$v/empty.tsv"                                         # zero bytes
printf 'a\n'                 > "$v/single-field.tsv"
printf 'a\tb\nc\td\n'        > "$v/basic.tsv"
printf 'a\t\tb\n'            > "$v/empty-fields.tsv"
printf 'a\tb\r\nc\td\r\n'    > "$v/crlf.tsv"
printf 'a\nb\r\n'            > "$v/mixed-endings.tsv"
printf 'a\tb\nc\td'          > "$v/no-trailing-newline.tsv"
printf 'a\n\nb\n'            > "$v/blank-line.tsv"
printf 'a\n\n'               > "$v/trailing-blank-line.tsv"
printf 'a\000b\tc\n'         > "$v/nul-byte.tsv"           # NUL is data
printf '\303\251\tb\n'       > "$v/utf8.tsv"
printf '\357\273\277a\tb\n'  > "$v/bom.tsv"
printf 'a\\tb\tc\n'          > "$v/backslash.tsv"          # a literal \t, not a TAB

# size and growth --------------------------------------------------------------
awk 'BEGIN { for (n = 1; n <= 1000; n++) printf "%s%d", (n > 1 ? "\t" : ""), n; print "" }' > "$v/wide-row.tsv" # forces realloc
awk 'BEGIN { for (n = 1; n <= 100000; n++) print n "\tx" }' > "$v/many-rows.tsv"
awk 'BEGIN { for (n = 0; n < 100000; n++) printf "x"; print "" }' > "$v/long-field.tsv"

# invalid input ----------------------------------------------------------------
printf 'a\tb\nc\n'           > "$i/ragged-short.tsv"
printf 'a\tb\nc\td\te\n'     > "$i/ragged-long.tsv"
printf 'a\tb\n\n'            > "$i/ragged-blank-line.tsv"  # a blank line is a 1-field row
printf 'a\rb\n'              > "$i/bare-cr.tsv"
printf 'a\r'                 > "$i/bare-cr-at-end.tsv"
printf 'a\rb\r'              > "$i/classic-mac.tsv"
