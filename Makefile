CC       ?= cc
CFLAGS   ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -g
CPPFLAGS += -Iinclude

build/tsviva: cli/main.c src/tsviva.c include/tsviva.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) cli/main.c src/tsviva.c -o build/tsviva

.PHONY: test
test: build/tsviva
	sh tests/run_tests.sh
