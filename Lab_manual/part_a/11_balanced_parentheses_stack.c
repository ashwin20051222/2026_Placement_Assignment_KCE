#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_SIZE = 1024
};

char stack[MAX_SIZE];
int  top = -1;

void push(char c) { stack[++top] = c; }
char pop_top()    { return stack[top--]; }
int  is_empty()   { return (top < 0); }

int is_open(char c) {
    return (c == '(' || c == '{' || c == '[');
}

int is_close(char c) {
    return (c == ')' || c == '}' || c == ']');
}

int matches(char open, char close) {
    if (open == '(' && close == ')') return 1;
    if (open == '{' && close == '}') return 1;
    if (open == '[' && close == ']') return 1;
    return 0;
}

int main() {
    char expr[MAX_SIZE];
    int i;

    printf("=== Balanced Parentheses Check using Stack ===\n");
    printf("Enter expression: ");
    fgets(expr, sizeof(expr), stdin);
    expr[strcspn(expr, "\n")] = 0;

    top = -1;
    int balanced = 1;

    for (i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];
        if (is_open(c)) {
            push(c);
        } else if (is_close(c)) {
            if (is_empty() || !matches(pop_top(), c)) {
                balanced = 0;
                break;
            }
        }
    }

    if (balanced && is_empty())
        printf("Result: BALANCED\n");
    else
        printf("Result: NOT BALANCED\n");

    return 0;
}
