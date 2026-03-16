#include <stdio.h>
#include <string.h>

#define MAX_SIZE 500

typedef struct {
    char items[MAX_SIZE];
    int top;
} Stack;

static void push(Stack *stack, char value) {
    if (stack->top < MAX_SIZE - 1) {
        stack->items[++stack->top] = value;
    }
}

static char pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->items[stack->top--];
    }
    return '\0';
}

int main(void) {
    char input[MAX_SIZE];
    Stack stack = {{0}, -1};

    printf("Enter a string: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }
    input[strcspn(input, "\n")] = '\0';

    for (size_t i = 0; input[i] != '\0'; ++i) {
        push(&stack, input[i]);
    }

    printf("Reversed string: ");
    while (stack.top >= 0) {
        putchar(pop(&stack));
    }
    printf("\n");

    return 0;
}
