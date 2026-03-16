#include <stdio.h>
#include <string.h>

#define MAX_EXPR 500

typedef struct {
    char items[MAX_EXPR];
    int top;
} Stack;

static void push(Stack *stack, char value) {
    if (stack->top < MAX_EXPR - 1) {
        stack->items[++stack->top] = value;
    }
}

static char pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->items[stack->top--];
    }
    return '\0';
}

static int is_matching(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

int main(void) {
    char expression[MAX_EXPR];
    Stack stack = {{0}, -1};
    int balanced = 1;

    printf("Enter an expression: ");
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        return 0;
    }

    for (size_t i = 0; expression[i] != '\0'; ++i) {
        char ch = expression[i];
        if (ch == '(' || ch == '[' || ch == '{') {
            push(&stack, ch);
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (stack.top == -1 || !is_matching(pop(&stack), ch)) {
                balanced = 0;
                break;
            }
        }
    }

    if (stack.top != -1) {
        balanced = 0;
    }

    if (balanced) {
        printf("The parentheses are balanced.\n");
    } else {
        printf("The parentheses are not balanced.\n");
    }

    return 0;
}
