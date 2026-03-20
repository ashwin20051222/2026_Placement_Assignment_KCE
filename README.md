# 2026 Placement Assignment KCE

This repository contains three C programming assignment sets covering embedded hardware programming, data structures, and operating systems concepts.

The codebase is organized as a collection of small, self-contained programs. Most files can be built and run independently, and some prebuilt artifacts are already included in the repository.

## Repository Structure

- `Ass_1_hw/`
  AVR/ATmega328P hardware-oriented programs written in C.
  Includes 35 assignment programs, 4 mini projects, a shared header, and a helper build script.
- `Lab_manual/`
  Lab-manual-based programs generated from `Lab_manual/Lab_Manual.doc`.
  - `Lab_manual/part_a/`: 15 standalone C programs (linked list/stack/queue/tree/graph/heap).
  - `Lab_manual/part_b/`: 15 standalone Linux IPC programs (pipes, message queues, shared memory, semaphores, threads, sockets).
  - `Lab_manual/part_c/`: 15 STM32F103RB Keil examples (assembly + peripheral demos; not built with gcc).
- `Ass_2_ds/`
  Data structures and algorithms programs written in standard C.
  Includes linked lists, stacks, queues, trees, hashing, sorting, binary search, file handling, and pthread-based examples.
- `Ass_3_os/`
  Linux operating systems and IPC programs written in C.
  Includes examples using pipes, shared memory, semaphores, threads, message queues, signals, and client-server style process communication.
- `01_Assignment_HW.pdf`
  Hardware assignment sheet.
- `02_Assginment_DS.pdf`
  Data structures assignment sheet.
- `03_Assignment_OS.pdf`
  Operating systems assignment sheet.

## Highlights

- `Ass_1_hw` includes GPIO, timers, PWM, ADC, interrupts, keypad, LCD, ultrasonic sensing, and mini project implementations for AVR microcontrollers.
- `Ass_2_ds` includes 33 standalone programs covering classic data structure problems and sorting/searching exercises.
- `Ass_3_os` includes 28 standalone Linux programs focused on IPC, process control, synchronization, and systems-style application design.
- Prebuilt `.elf` and `.hex` files are present for many AVR programs, and compiled binaries are present for several desktop C programs.

## Requirements

For `Ass_1_hw`:

- `avr-gcc`
- `avr-objcopy`
- `avr-size`
- An AVR-compatible environment if you want to flash or simulate the generated binaries

For `Ass_2_ds` and `Ass_3_os`:

- `gcc`
- `make`
- A Linux or POSIX-compatible environment
- `pthread` support for multithreaded programs

## Build Instructions

### 1. Hardware Assignments (`Ass_1_hw`)

Build all AVR source files and mini projects:

```bash
cd Ass_1_hw
bash compile_all.sh
```

Notes:

- Default MCU: `atmega328p`
- Default clock: `16000000UL`
- Output files are generated as `.elf` and `.hex`
- You can override toolchain variables such as `MCU`, `F_CPU`, and `TOOLCHAIN_PREFIX`

Example:

```bash
cd Ass_1_hw
MCU=atmega328p F_CPU=16000000UL bash compile_all.sh
```

### 2. Data Structures Assignments (`Ass_2_ds`)

Compile any standalone source file with `gcc`.

Example:

```bash
cd Ass_2_ds
gcc -std=c11 -Wall -Wextra -pedantic 31_library_book_search.c -o 31_library_book_search
./31_library_book_search
```

Some binaries are already present in this folder and can be run directly if they match your platform.

### 3. Operating Systems Assignments (`Ass_3_os`)

Build all OS programs:

```bash
cd Ass_3_os
make
```

List available targets:

```bash
cd Ass_3_os
make list
```

Run a sample program:

```bash
cd Ass_3_os
./1_thermostat_compressor_pipe_control
```

Clean generated binaries:

```bash
cd Ass_3_os
make clean
```

### 4. Lab Manual Programs (`Lab_manual`)

Part A build:

```bash
cd Lab_manual/part_a
mkdir -p bin
for f in *.c; do gcc -std=c11 -Wall -Wextra -Wpedantic -O2 "$f" -o "bin/${f%.c}"; done
```

Part B build (Linux IPC; `-pthread` for thread/shared-mutex examples):

```bash
cd Lab_manual/part_b
mkdir -p bin
for f in *.c; do
  if [ "$f" = "07_shared_memory_time_temp_humidity_display.c" ] || [ "$f" = "12_threads_shm_mutex_logger_display.c" ]; then
    gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2 "$f" -o "bin/${f%.c}" -pthread
  else
    gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2 "$f" -o "bin/${f%.c}"
  fi
done
```

Part C (Keil / STM32F103RB):
- See `Lab_manual/part_c/README_KEIL_PART_C.md`

## Program Style

- Most files are single-program C source files with `main()`
- AVR code uses `<avr/io.h>` and targets ATmega-class microcontrollers
- Linux systems programs rely on POSIX APIs such as `fork`, `pipe`, `waitpid`, shared memory, and threads
- File names are descriptive and map closely to the problem they solve

## Suggested Workflow

1. Read the relevant PDF assignment sheet.
2. Open the matching folder (`Ass_1_hw`, `Ass_2_ds`, or `Ass_3_os`).
3. Build either one program or the whole assignment set.
4. Run the executable locally, or generate `.hex` files for AVR deployment/simulation.

## Notes

- This repository contains both source code and generated build outputs.
- Some programs are intended for AVR hardware targets and cannot be executed directly on a regular desktop system.
- Some Linux assignment programs may require terminal interaction or specific runtime behavior to demonstrate IPC features.

