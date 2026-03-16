#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HistoryNode {
    char path[256];
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static HistoryNode *create_node(const char *path) {
    HistoryNode *node = (HistoryNode *) malloc(sizeof(HistoryNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(node->path, path, sizeof(node->path) - 1);
    node->path[sizeof(node->path) - 1] = '\0';
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static void free_forward(HistoryNode *node) {
    while (node != NULL) {
        HistoryNode *next = node->next;
        free(node);
        node = next;
    }
}

static void visit_directory(HistoryNode **current, const char *path) {
    HistoryNode *new_node = create_node(path);

    if (*current == NULL) {
        *current = new_node;
        return;
    }

    free_forward((*current)->next);
    (*current)->next = new_node;
    new_node->prev = *current;
    *current = new_node;
}

static void show_history(HistoryNode *current) {
    if (current == NULL) {
        printf("History is empty.\n");
        return;
    }

    while (current->prev != NULL) {
        current = current->prev;
    }

    printf("\nDirectory history:\n");
    while (current != NULL) {
        printf("%s\n", current->path);
        current = current->next;
    }
}

static void free_all(HistoryNode *current) {
    if (current == NULL) {
        return;
    }

    while (current->prev != NULL) {
        current = current->prev;
    }
    free_forward(current);
}

int main(void) {
    HistoryNode *current = NULL;
    int choice;
    char path[256];

    while (1) {
        printf("\nDirectory History Menu\n");
        printf("1. Visit directory\n");
        printf("2. Go to previous directory\n");
        printf("3. Go to next directory\n");
        printf("4. Show current directory\n");
        printf("5. Show full history\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter directory path: ");
                read_line(path, sizeof(path));
                visit_directory(&current, path);
                break;
            case 2:
                if (current != NULL && current->prev != NULL) {
                    current = current->prev;
                    printf("Moved to: %s\n", current->path);
                } else {
                    printf("No previous directory.\n");
                }
                break;
            case 3:
                if (current != NULL && current->next != NULL) {
                    current = current->next;
                    printf("Moved to: %s\n", current->path);
                } else {
                    printf("No next directory.\n");
                }
                break;
            case 4:
                if (current != NULL) {
                    printf("Current directory: %s\n", current->path);
                } else {
                    printf("History is empty.\n");
                }
                break;
            case 5:
                show_history(current);
                break;
            case 6:
                free_all(current);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_all(current);
    return 0;
}
