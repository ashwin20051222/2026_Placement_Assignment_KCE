#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

MCU="${MCU:-atmega328p}"
F_CPU="${F_CPU:-16000000UL}"
TOOLCHAIN_PREFIX="${TOOLCHAIN_PREFIX:-avr-}"
CC="${CC:-${TOOLCHAIN_PREFIX}gcc}"
OBJCOPY="${OBJCOPY:-${TOOLCHAIN_PREFIX}objcopy}"
SIZE="${SIZE:-${TOOLCHAIN_PREFIX}size}"
CFLAGS="${CFLAGS:--std=c11 -Os -Wall -Wextra -mmcu=${MCU}}"
CPPFLAGS="${CPPFLAGS:- -DF_CPU=${F_CPU}}"
LDFLAGS="${LDFLAGS:- -mmcu=${MCU}}"

for tool in "$CC" "$OBJCOPY" "$SIZE"; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "error: $tool not found. Install the AVR GCC toolchain first."
        exit 1
    fi
done

shopt -s nullglob
sources=(
    "$script_dir"/[0-9]*_*.c
    "$script_dir"/mini_projects/*.c
)

if [ "${#sources[@]}" -eq 0 ]; then
    echo "error: no C source files found to compile."
    exit 1
fi

for src in "${sources[@]}"; do
    rel_path="${src#"$script_dir"/}"
    obj="$script_dir/.build/${rel_path%.c}.o"
    elf="${src%.c}.elf"
    hex="${src%.c}.hex"

    mkdir -p "$(dirname "$obj")"

    echo "Compiling $rel_path"
    "$CC" $CPPFLAGS $CFLAGS -c "$src" -o "$obj"
    "$CC" $LDFLAGS "$obj" -o "$elf"
    "$SIZE" "$elf"
    "$OBJCOPY" -O ihex -R .eeprom "$elf" "$hex"
done

echo "Generated separate .elf and .hex files for ${#sources[@]} programs."
