#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char path[256];
    struct Node *prev;
    struct Node *next;
};

struct Node *node_new(char *path) {
    struct Node *n = (struct Node *)malloc(sizeof(struct Node));
    if (n == NULL) return NULL;
    strcpy(n->path, path);
    n->prev = NULL;
    n->next = NULL;
    return n;
}

void free_forward(struct Node *n) {
    while (n != NULL) {
        struct Node *next = n->next;
        free(n);
        n = next;
    }
}

int main() {
    struct Node *head    = NULL;
    struct Node *current = NULL;
    int choice;

    printf("=== Directory History (Doubly Linked List) ===\n");

    while (1) {
        printf("\n1. Visit new directory\n");
        printf("2. Go Back\n");
        printf("3. Go Forward\n");
        printf("4. Show history\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char path[256];
            printf("Enter directory path: ");
            fgets(path, sizeof(path), stdin);
            path[strcspn(path, "\n")] = 0;

            struct Node *n = node_new(path);
            if (n == NULL) { printf("Memory error.\n"); continue; }

            if (head == NULL) {

                head = n;
                current = n;
            } else {

                if (current != NULL && current->next != NULL) {
                    free_forward(current->next);
                    current->next = NULL;
                }
                n->prev = current;
                if (current != NULL) current->next = n;
                current = n;
            }
            printf("Current: %s\n", current->path);

        } else if (choice == 2) {
            if (current != NULL && current->prev != NULL) {
                current = current->prev;
                printf("Current: %s\n", current->path);
            } else {
                printf("No previous directory.\n");
            }

        } else if (choice == 3) {
            if (current != NULL && current->next != NULL) {
                current = current->next;
                printf("Current: %s\n", current->path);
            } else {
                printf("No next directory.\n");
            }

        } else if (choice == 4) {
            printf("\n--- History ---\n");
            if (head == NULL) { printf("(empty)\n"); continue; }
            struct Node *p = head;
            int idx = 1;
            while (p != NULL) {
                char marker = (p == current) ? '>' : ' ';
                printf("%c %2d. %s\n", marker, idx++, p->path);
                p = p->next;
            }

        } else {
            printf("Invalid choice.\n");
        }
    }

    while (head != NULL) {
        struct Node *n = head->next;
        free(head);
        head = n;
    }
    return 0;
}
