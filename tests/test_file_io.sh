#!/bin/sh
set -eu

ROOT=$(CDPATH= cd "$(dirname "$0")/.." && pwd)
PROGRAM="$ROOT/build/tsviva"
FIXTURE="$ROOT/tests/fixtures/hello.txt"

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

# Run the program and capture its output.
"$PROGRAM" "$FIXTURE" > "$TMP/actual"

# Expected output is the fixture contents plus main.c's final newline.
cat "$FIXTURE" > "$TMP/expected"
printf '\n' >> "$TMP/expected"

# Compare byte-for-byte.
if cmp -s "$TMP/expected" "$TMP/actual"; then
    echo "PASS: file input is read and printed correctly"
else
    echo "FAIL: output did not match expected contents"
    diff -u "$TMP/expected" "$TMP/actual" || true
    exit 1
fi