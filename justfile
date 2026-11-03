# SPDX-FileCopyrightText: 2026 Jiffly
# SPDX-License-Identifier: MIT OR Apache-2.0

_default:
    just --list

build:
    cargo build

test:
    cargo test

lint: c-lint
    cargo clippy --all-targets -- --deny warnings
    cargo fmt --check

reuse:
    reuse lint

c-test:
    make check CC=gcc BUILD_DIR=target/c-gcc
    make check CC=clang BUILD_DIR=target/c-clang

c-sanitize:
    make check CC=clang BUILD_DIR=target/c-sanitize SANITIZE=1

c-lint:
    make lint

c-fmt:
    make format

abi-c-check: c-lint c-test c-sanitize

check: lint build test reuse c-test c-sanitize

clean:
    cargo clean

fmt: c-fmt
    cargo fmt
