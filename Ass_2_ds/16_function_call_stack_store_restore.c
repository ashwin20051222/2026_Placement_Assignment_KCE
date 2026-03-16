#include <stdio.h>

#define MAX_DEPTH 100

typedef struct {
    char function_name[50];
    int argument;
} CallFrame;

typedef struct {
    CallFrame frames[MAX_DEPTH];
    int top;
} CallStack;

static void push(CallStack *stack, const char *name, int argument) {
    if (stack->top >= MAX_DEPTH - 1) {
        return;
    }
    ++stack->top;
    snprintf(stack->frames[stack->top].function_name,
             sizeof(stack->frames[stack->top].function_name),
             "%s",
             name);
    stack->frames[stack->top].argument = argument;
}

static void pop(CallStack *stack) {
    if (stack->top >= 0) {
        --stack->top;
    }
}

static void display_stack(const CallStack *stack) {
    printf("Current call stack:\n");
    for (int i = stack->top; i >= 0; --i) {
        printf("  %s(%d)\n", stack->frames[i].function_name, stack->frames[i].argument);
    }
}

static int sum_to_n(int n, CallStack *stack) {
    push(stack, "sum_to_n", n);
    printf("\nStored data while entering function call:\n");
    display_stack(stack);

    int result;
    if (n == 0) {
        result = 0;
    } else {
        result = n + sum_to_n(n - 1, stack);
    }

    printf("Restoring data while returning from %s(%d)\n",
           stack->frames[stack->top].function_name,
           stack->frames[stack->top].argument);
    pop(stack);
    return result;
}

int main(void) {
    int n;
    CallStack stack = {.top = -1};

    printf("Enter n to calculate sum from 1 to n: ");
    scanf("%d", &n);

    if (n < 0) {
        printf("n must be non-negative.\n");
        return 0;
    }

    printf("Result: %d\n", sum_to_n(n, &stack));
    return 0;
}
