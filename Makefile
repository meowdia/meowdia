# SPDX-FileCopyrightText: 2026 Jiffly
# SPDX-License-Identifier: MIT OR Apache-2.0

BUILD_DIR ?= target/c
PREFIX ?= /usr/local
SANITIZE ?= 0

HEADERS := $(wildcard include/meow/*.h)
TESTS := $(wildcard tests/abi/c/*.c)
SOURCES := sdk/c/checks.c $(TESTS)
C_FLAGS := -std=c11 -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion \
	-Wshadow -Wformat=2 -Wundef -Wvla -Wcast-qual -Wwrite-strings \
	-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition \
	-fPIC -fvisibility=hidden
ifeq ($(SANITIZE),1)
C_FLAGS += -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
endif
COMPILE_FLAGS = $(CPPFLAGS) -Iinclude $(CFLAGS) $(C_FLAGS)

.PHONY: all check format lint install
all:
	mkdir -p $(BUILD_DIR)
	$(CC) $(COMPILE_FLAGS) -c sdk/c/checks.c -o $(BUILD_DIR)/checks.o
	rm -f $(BUILD_DIR)/libmeow-sdk.a
	$(AR) rcs $(BUILD_DIR)/libmeow-sdk.a $(BUILD_DIR)/checks.o

check: all
	@set -e; for header in $(HEADERS); do \
		printf '#include "%s"\n#include "%s"\n' "$$header" "$$header" | \
			$(CC) $(COMPILE_FLAGS) -I. -x c -c -o /dev/null -; \
	done
	$(CC) $(COMPILE_FLAGS) -UNDEBUG $(TESTS) \
		$(BUILD_DIR)/libmeow-sdk.a $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)/abi-checks
	$(BUILD_DIR)/abi-checks

format:
	clang-format -i $(HEADERS) $(SOURCES)

lint:
	clang-format --dry-run --Werror $(HEADERS) $(SOURCES)
	clang-tidy --config-file=.clang-tidy $(SOURCES) -- $(COMPILE_FLAGS) -UNDEBUG

install: all
	install -Dm644 $(BUILD_DIR)/libmeow-sdk.a -t $(DESTDIR)$(PREFIX)/lib
	install -Dm644 $(HEADERS) -t $(DESTDIR)$(PREFIX)/include/meow
	install -Dm644 LICENSES/* -t $(DESTDIR)$(PREFIX)/share/licenses/meow-c-sdk
