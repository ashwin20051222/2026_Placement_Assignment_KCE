#include <stdio.h>
#include <string.h>

#define MAX_BLOCKS 100
#define TOTAL_MEMORY 1024

typedef struct {
    char process_name[100];
    int start_address;
    int size;
} MemoryBlock;

typedef struct {
    MemoryBlock blocks[MAX_BLOCKS];
    int top;
    int used_memory;
} MemoryStack;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static void allocate_memory(MemoryStack *stack, const char *name, int size) {
    if (stack->top >= MAX_BLOCKS - 1) {
        printf("Memory stack is full.\n");
        return;
    }

    if (size <= 0 || stack->used_memory + size > TOTAL_MEMORY) {
        printf("Allocation failed. Not enough memory.\n");
        return;
    }

    ++stack->top;
    strncpy(stack->blocks[stack->top].process_name, name,
            sizeof(stack->blocks[stack->top].process_name) - 1);
    stack->blocks[stack->top].process_name[sizeof(stack->blocks[stack->top].process_name) - 1] = '\0';
    stack->blocks[stack->top].start_address = stack->used_memory;
    stack->blocks[stack->top].size = size;
    stack->used_memory += size;

    printf("Allocated %d units to %s at address %d\n",
           size,
           stack->blocks[stack->top].process_name,
           stack->blocks[stack->top].start_address);
}

static void deallocate_memory(MemoryStack *stack) {
    if (stack->top < 0) {
        printf("No memory block to deallocate.\n");
        return;
    }

    MemoryBlock block = stack->blocks[stack->top--];
    stack->used_memory -= block.size;
    printf("Deallocated %s of size %d\n", block.process_name, block.size);
}

static void display_memory(const MemoryStack *stack) {
    if (stack->top < 0) {
        printf("No memory allocations present.\n");
        return;
    }

    printf("\nAllocated memory blocks:\n");
    for (int i = stack->top; i >= 0; --i) {
        printf("Process: %-15s Start: %-4d Size: %d\n",
               stack->blocks[i].process_name,
               stack->blocks[i].start_address,
               stack->blocks[i].size);
    }
    printf("Used memory: %d / %d\n", stack->used_memory, TOTAL_MEMORY);
}

int main(void) {
    MemoryStack stack = {.top = -1, .used_memory = 0};
    int choice;
    int size;
    char name[100];

    while (1) {
        printf("\nMemory Management Menu\n");
        printf("1. Allocate memory\n");
        printf("2. Deallocate memory\n");
        printf("3. Display memory stack\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter process name: ");
                read_line(name, sizeof(name));
                printf("Enter memory size to allocate: ");
                scanf("%d", &size);
                getchar();
                allocate_memory(&stack, name, size);
                break;
            case 2:
                deallocate_memory(&stack);
                break;
            case 3:
                display_memory(&stack);
                break;
            case 4:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
