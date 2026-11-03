# SPDX-FileCopyrightText: 2026 Jiffly
# SPDX-License-Identifier: MIT OR Apache-2.0

test:
    make check CC=gcc BUILD_DIR=target/c-gcc
    make check CC=clang BUILD_DIR=target/c-clang

sanitize:
    make check CC=clang BUILD_DIR=target/c-sanitize SANITIZE=1

lint:
    make lint

fmt:
    make format

check: lint test sanitize
