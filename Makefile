CC       ?= cc
CFLAGS   ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -g
CPPFLAGS += -Iinclude

build/tsviva: cli/main.c src/tsviva.c include/tsviva.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) cli/main.c src/tsviva.c -o build/tsviva

.PHONY: test
test: build/tsviva
	sh tests/run_tests.sh

build/tsviva-asan: cli/main.c src/tsviva.c include/tsviva.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -fsanitize=address,undefined \
		-fno-sanitize-recover=undefined -fno-omit-frame-pointer \
		cli/main.c src/tsviva.c -o build/tsviva-asan

.PHONY: asan
asan: build/tsviva-asan
	ASAN_OPTIONS=exitcode=86 UBSAN_OPTIONS=exitcode=86 \
		CLI=build/tsviva-asan sh tests/run_tests.sh

.PHONY: clean
clean:
	rm -rf build
