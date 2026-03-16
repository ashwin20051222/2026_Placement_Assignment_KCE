#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT 1000

typedef struct StateNode {
    char state[MAX_TEXT];
    struct StateNode *next;
} StateNode;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static void push_state(StateNode **top, const char *text) {
    StateNode *node = (StateNode *) malloc(sizeof(StateNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(node->state, text, sizeof(node->state) - 1);
    node->state[sizeof(node->state) - 1] = '\0';
    node->next = *top;
    *top = node;
}

static int pop_state(StateNode **top, char *text) {
    if (*top == NULL) {
        return 0;
    }

    StateNode *node = *top;
    *top = node->next;
    strncpy(text, node->state, MAX_TEXT - 1);
    text[MAX_TEXT - 1] = '\0';
    free(node);
    return 1;
}

static void free_states(StateNode *top) {
    while (top != NULL) {
        StateNode *next = top->next;
        free(top);
        top = next;
    }
}

int main(void) {
    StateNode *undo_stack = NULL;
    char document[MAX_TEXT] = "";
    char text[MAX_TEXT];
    int choice;
    int count;

    while (1) {
        printf("\nUndo Stack Menu\n");
        printf("1. Append text\n");
        printf("2. Delete last N characters\n");
        printf("3. Undo last operation\n");
        printf("4. Display document\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                push_state(&undo_stack, document);
                printf("Enter text to append: ");
                read_line(text, sizeof(text));
                if (strlen(document) + strlen(text) < MAX_TEXT) {
                    strcat(document, text);
                } else {
                    printf("Append skipped: document would exceed limit.\n");
                }
                break;
            case 2:
                push_state(&undo_stack, document);
                printf("Enter number of characters to delete: ");
                scanf("%d", &count);
                getchar();
                if (count < 0) {
                    count = 0;
                }
                if ((size_t) count > strlen(document)) {
                    count = (int) strlen(document);
                }
                document[strlen(document) - (size_t) count] = '\0';
                break;
            case 3:
                if (!pop_state(&undo_stack, document)) {
                    printf("Nothing to undo.\n");
                }
                break;
            case 4:
                printf("Document: %s\n", document);
                break;
            case 5:
                free_states(undo_stack);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_states(undo_stack);
    return 0;
}
