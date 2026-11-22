# tsviva Specification

This document specifies the input accepted by tsviva and how the parser
handles it.

"Must" indicates a rule the parser enforces. "May" indicates something the
parser accepts.

## Notation

Input is a sequence of bytes. Control characters are written in angle
brackets.

| Notation | Byte   | Name            |
|----------|--------|-----------------|
| `<TAB>`  | `0x09` | horizontal tab  |
| `<LF>`   | `0x0A` | line feed       |
| `<CR>`   | `0x0D` | carriage return |

Control bytes are written in angle brackets rather than with backslash
sequences. Backslashes have no special meaning to the parser.

## Grammar

The grammar below defines the accepted input. TAB, LF, and CR refer to the
bytes listed above.

```
table      = { row terminator } [ final_row ]
row        = field { TAB field }
final_row  = row (* must contain at least one byte of input *)
field      = { byte - (TAB | LF | CR) }
terminator = LF | CR LF
```

The input is a table made up of rows. Each row contains one or more fields. The
last row may omit its terminator, but it must contain at least one byte. A
trailing terminator does not produce an extra row.

- A table may have zero rows (empty input).
- A row always has at least one field.
- Rows may use different terminators within one table.

| Input                 | Result                              |
|-----------------------|-------------------------------------|
| (zero bytes)          | zero rows                           |
| `a<LF>`               | one row: `a`                        |
| `a`                   | one row: `a`                        |
| `a<LF>b<CR><LF>`      | two rows: `a`; `b`                  |
| `a<CR>b<LF>`          | error (bare `<CR>`)                 |

## Encoding

The parser treats input as bytes. It does not validate or interpret character
encoding.

A byte order mark has no special meaning. If present, it is part of the
first field.

The byte `0x00` (NUL) is an ordinary byte and is returned as field
content. Fields are therefore a pointer and a length, not C strings.

## Empty fields

A field may contain zero bytes. Adjacent `<TAB>` bytes therefore delimit
an empty field between them.

| Input                    | Fields                |
|--------------------------|-----------------------|
| `a<TAB><TAB>b<LF>`       | `a`, (empty), `b`     |
| `<TAB><LF>`              | (empty), (empty)      |

## Blank lines

A line with no bytes before its terminator is a row containing one empty
field. The parser must not skip it.

| Input              | Result                          |
|--------------------|---------------------------------|
| `<LF>`             | one row: (empty)                |
| `a<LF><LF>b<LF>`   | three rows: `a`; (empty); `b`   |
| (zero bytes)       | zero rows                       |

Empty input and a lone `<LF>` are different: the first has no rows, and
the second has one row with one empty field. A table that ends with two
terminators therefore ends with an empty row.

## Line endings

A row ends with either `<LF>` or `<CR><LF>`. Both are accepted, and rows
within one table may use either terminator.

A `<CR>` that is not immediately followed by `<LF>` is an error. The parser
does not treat it as field content.

| Input                | Result                     |
|----------------------|----------------------------|
| `a<LF>b<LF>`         | two rows: `a`; `b`         |
| `a<CR><LF>b<CR><LF>` | two rows: `a`; `b`         |
| `a<LF>b<CR><LF>`     | two rows: `a`; `b`         |
| `a<CR>b<LF>`         | error (bare `<CR>`)        |
| `a<CR>`              | error (bare `<CR>` at end) |

## Ragged rows

The first row sets the table's width, measured in fields. By default, later
rows must match that field count. The parser stops at the first row with a
different field count and returns an error giving the row number (counting from
1), the expected width, and the actual width.

A caller may disable this check. With it disabled, rows of any width are
returned as they are.

| Input                          | Check on                              | Check off                               |
|--------------------------------|---------------------------------------|-----------------------------------------|
| `a<TAB>b<LF>c<TAB>d<LF>`       | two rows: `a`, `b`; `c`, `d`          | two rows: `a`, `b`; `c`, `d`            |
| `a<TAB>b<LF>c<LF>`             | error: row 2 has 1 field, expected 2  | two rows: `a`, `b`; `c`                 |
| `a<TAB>b<LF>c<TAB>d<TAB>e<LF>` | error: row 2 has 3 fields, expected 2 | two rows: `a`, `b`; `c`, `d`, `e`       |
| `a<TAB>b<LF><LF>`              | error: row 2 has 1 field, expected 2  | two rows: `a`, `b`; (empty)             |
| `a<LF><LF>`                    | two rows: `a`; (empty)                | two rows: `a`; (empty)                  |

## Escapes

The parser does not interpret backslashes. A backslash is an ordinary byte and
is returned as part of its field. In particular, `\t` in the input is two
bytes, a backslash followed by `t`, and is not a `<TAB>`.

A field cannot contain `<TAB>`, `<LF>`, or `<CR>`.

| Input              | Result                                     |
|--------------------|--------------------------------------------|
| `a\tb<LF>`         | one row, one field: `a\tb` (4 bytes)       |
| `a\<TAB>b<LF>`     | one row, two fields: `a\`; `b`             |

## Errors

The parser reports an error for the following:

1. A `<CR>` not immediately followed by `<LF>` (see Line endings).
2. A row whose field count differs from the first row's, when the width check
   is enabled (see Ragged rows).

Every format error reports the row number, counting from 1. A bare <CR> error
also reports its byte offset, counting from 0.

The parser stops at the first error and does not return a partial table.
Failure to allocate memory, or to read the input, is reported separately from
format errors.

## Non-goals

tsviva does not:

- interpret escape sequences or support quoted fields;
- treat the first row as a header;
- assign meaning to fields or convert them to other types;
- support delimiters other than `<TAB>`;
- validate character encodings;
- write TSV.
