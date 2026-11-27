[![CI](https://github.com)](https://github.com)
[![License: MIT](https://shields.io)](https://opensource.org)

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
