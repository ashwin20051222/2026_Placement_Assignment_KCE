#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_SIZE = 1024
};

char stack[MAX_SIZE];
int  top = -1;

void push(char c) {
    if (top >= MAX_SIZE - 1) { printf("Stack overflow!\n"); return; }
    stack[++top] = c;
}

char pop() {
    if (top < 0) return '\0';
    return stack[top--];
}

int is_empty() {
    return (top < 0);
}

int main() {
    char input[MAX_SIZE];
    int i;

    printf("=== String Reversal using Stack ===\n");
    printf("Enter string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    int len = (int)strlen(input);

    for (i = 0; i < len; i++)
        push(input[i]);

    printf("Reversed   : ");
    while (!is_empty())
        putchar(pop());
    printf("\n");

    return 0;
}
