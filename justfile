# SPDX-FileCopyrightText: 2026 Jiffly
# SPDX-License-Identifier: MIT OR Apache-2.0

mod c 'c.just'

_default:
    just --list

build:
    cargo build

test:
    cargo test

lint: c::lint
    cargo clippy --all-targets -- --deny warnings
    cargo fmt --check

reuse:
    reuse lint

check: lint build test reuse c::test c::sanitize

clean:
    cargo clean

fmt: c::fmt
    cargo fmt
