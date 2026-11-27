# tsviva

A lightweight, C11-compliant TSV (Tab-Separated Values) parser library and CLI tool.

[![CI](https://github.com/jamie-e-evans/tsviva/actions/workflows/ci.yml/badge.svg)](https://github.com/jamie-e-evans/tsviva/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Build & Test

Requires a C11-compatible compiler (`gcc` or `clang`) and `make`.

```sh
# Build the CLI binary
make

# Run the test suite
make test

# Build and test with AddressSanitizer and UndefinedBehaviorSanitizer
make asan

# Remove build artifacts
make clean
```

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
