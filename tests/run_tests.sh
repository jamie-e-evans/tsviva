#!/bin/sh
# Run from the project root: sh tests/run_tests.sh
# Valid fixtures must exit 0, invalid ones 1. When a .expected file sits next
# to a fixture, the CLI's stdout has to match it exactly.
cli=build/tsviva
pass=0
failed=0

check() {
    f=$1
    want=$2
    out=$(mktemp)
    "$cli" "$f" > "$out" 2>/dev/null
    got=$?
    ok=1
    if [ "$got" -ne "$want" ]; then
        echo "FAIL $f: exit $got, wanted $want"
        ok=0
    fi
    exp="${f%.tsv}.expected"
    if [ -f "$exp" ] && ! cmp -s "$out" "$exp"; then
        echo "FAIL $f: output differs from $exp"
        ok=0
    fi
    rm -f "$out"
    if [ "$ok" -eq 1 ]; then
        pass=$((pass + 1))
    else
        failed=$((failed + 1))
    fi
}

for f in tests/fixtures/valid/*.tsv; do check "$f" 0; done
for f in tests/fixtures/invalid/*.tsv; do check "$f" 1; done

echo "$pass passed, $failed failed"
[ "$failed" -eq 0 ]
