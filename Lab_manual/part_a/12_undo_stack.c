#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_TEXT = 1024,
    MAX_UNDO = 1024
};

enum {
    OP_APPEND = 1,
    OP_DELETE = 2
};

struct Op {
    int  type;
    char ch;
};

char text_buf[MAX_TEXT];
int  text_len = 0;

struct Op undo_stack[MAX_UNDO];
int       undo_top = -1;

void push_undo(int type, char ch) {
    if (undo_top >= MAX_UNDO - 1) { printf("Undo stack full.\n"); return; }
    undo_top++;
    undo_stack[undo_top].type = type;
    undo_stack[undo_top].ch   = ch;
}

void do_append(char c) {
    if (text_len >= MAX_TEXT - 1) { printf("Text buffer full.\n"); return; }
    text_buf[text_len++] = c;
    text_buf[text_len]   = '\0';
    push_undo(OP_APPEND, c);
    printf("Appended '%c'. Text: \"%s\"\n", c, text_buf);
}

void do_delete() {
    if (text_len == 0) { printf("Text is empty, nothing to delete.\n"); return; }
    char removed = text_buf[--text_len];
    text_buf[text_len] = '\0';
    push_undo(OP_DELETE, removed);
    printf("Deleted '%c'. Text: \"%s\"\n", removed, text_buf);
}

void do_undo() {
    if (undo_top < 0) { printf("Nothing to undo.\n"); return; }
    struct Op op = undo_stack[undo_top--];
    if (op.type == OP_APPEND) {

        if (text_len > 0) text_buf[--text_len] = '\0';
        printf("Undo append '%c'. Text: \"%s\"\n", op.ch, text_buf);
    } else {

        if (text_len < MAX_TEXT - 1) {
            text_buf[text_len++] = op.ch;
            text_buf[text_len]   = '\0';
        }
        printf("Undo delete '%c'. Text: \"%s\"\n", op.ch, text_buf);
    }
}

int main() {
    int choice;
    char ch;

    text_buf[0] = '\0';
    printf("=== Text Editor with Undo Stack ===\n");
    printf("Current text: \"%s\"\n", text_buf);

    while (1) {
        printf("\n1. Append character\n");
        printf("2. Delete last character\n");
        printf("3. Undo\n");
        printf("4. Show text\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            printf("Character to append: ");
            scanf("%c", &ch); getchar();
            do_append(ch);

        } else if (choice == 2) {
            do_delete();

        } else if (choice == 3) {
            do_undo();

        } else if (choice == 4) {
            printf("Current text: \"%s\"\n", text_buf);

        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}
