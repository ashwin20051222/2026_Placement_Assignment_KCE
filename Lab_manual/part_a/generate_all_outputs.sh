#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BIN_DIR="$ROOT_DIR/bin"
OUT_DIR="$ROOT_DIR/generated_outputs"

mkdir -p "$BIN_DIR" "$OUT_DIR"

compile_all() {
  local src
  local base

  for src in "$ROOT_DIR"/*.c; do
    base="$(basename "${src%.c}")"
    gcc -std=c11 -Wall -Wextra -O2 "$src" -o "$BIN_DIR/$base"
  done
}

run_case() {
  local name="$1"
  shift

  {
    printf '$'
    printf ' %q' "$@"
    printf '\n\n'
    "$@"
  } >"$OUT_DIR/$name.txt" 2>&1 || true
}

write_summary() {
  cat >"$OUT_DIR/README.txt" <<'EOF'
These files were generated locally from the programs in this folder.

Interpretation used for "all possible":
- each interactive program was exercised through its main visible paths
- empty-state and boundary/not-found behavior were included where practical
- programs with one-shot input were given separate success/failure examples when useful

Open the individual .txt files in this directory to see the captured terminal output.
EOF
}

compile_all

run_case "01_playlist_linked_list" "$BIN_DIR/01_playlist_linked_list" <<'EOF'
6
4
1
Alpha
Artist A
210
1
Beta
Artist B
180
6
3
Gamma
3
Alpha
4
4
5
5
2
Gamma
2
Alpha
6
0
EOF

run_case "02_conga_line_linked_list" "$BIN_DIR/02_conga_line_linked_list" <<'EOF'
4
3
Zed
1
Alice
1
Bob
1
Cara
4
3
Bob
2
Zoe
2
Cara
4
2
Alice
2
Bob
4
0
EOF

run_case "03_treasure_hunt_linked_list" "$BIN_DIR/03_treasure_hunt_linked_list" <<'EOF'
2
3
1
Library
Find the red book
1
Garden
Look under the bench
2
3

4
2
4
2
4
2
0
EOF

run_case "04_josephus_linked_list_valid" "$BIN_DIR/04_josephus_linked_list" <<'EOF'
7
3
EOF

run_case "04_josephus_linked_list_invalid" "$BIN_DIR/04_josephus_linked_list" <<'EOF'
0
2
EOF

run_case "05_directory_history_dll" "$BIN_DIR/05_directory_history_dll" <<'EOF'
4
2
3
1
/home
1
/usr/local
1
/tmp
4
2
2
2
3
1
/opt/project
4
3
0
EOF

run_case "06_hash_table_chaining" "$BIN_DIR/06_hash_table_chaining" <<'EOF'
4
2
z
1
a
10
1
l
20
4
2
a
1
a
15
2
a
3
x
3
l
4
0
EOF

run_case "07_car_lookup_tree_bst" "$BIN_DIR/07_car_lookup_tree_bst" <<'EOF'
3
2
Corolla
1
Tesla Model Y
Tesla
SUV
1
Civic
Honda
Sedan
1
Tesla Model Y
Tesla Inc
Crossover
2
Tesla Model Y
2
Corolla
3
0
EOF

run_case "08_graph_adjacency_list_valid" "$BIN_DIR/08_graph_adjacency_list" <<'EOF'
5
1
3
0 1
1 5
1 2
3 4
0
EOF

run_case "08_graph_adjacency_list_invalid_count" "$BIN_DIR/08_graph_adjacency_list" <<'EOF'
0
EOF

run_case "09_scheduler_linked_list" "$BIN_DIR/09_scheduler_linked_list" <<'EOF'
2
3
1
Compile
50
1
Idle
0
1
Broken
-5
2
3
0
EOF

run_case "10_string_reverse_stack" "$BIN_DIR/10_string_reverse_stack" <<'EOF'
Data Structures
EOF

run_case "11_balanced_parentheses_stack_balanced" "$BIN_DIR/11_balanced_parentheses_stack" <<'EOF'
([a+b]*{c-d})
EOF

run_case "11_balanced_parentheses_stack_unbalanced" "$BIN_DIR/11_balanced_parentheses_stack" <<'EOF'
([)]
EOF

run_case "12_undo_stack" "$BIN_DIR/12_undo_stack" <<'EOF'
4
2
3
1
a
1
b
4
2
3
3
4
0
EOF

run_case "13_token_queue" "$BIN_DIR/13_token_queue" <<'EOF'
3
2
1
hello world queue
3
2
3
4
2
0
EOF

run_case "14_ticket_counter_queue" "$BIN_DIR/14_ticket_counter_queue" <<'EOF'
3
2
1
Alice
5
1
Bob
3
3
2
3
2
2
0
EOF

run_case "15_priority_scheduling_heapsort" "$BIN_DIR/15_priority_scheduling_heapsort" <<'EOF'
2
3
1
Render
3
40
1
Input
1
20
1
Save
2
10
2
3
0
EOF

write_summary

printf 'Generated transcripts in %s\n' "$OUT_DIR"
