#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Clue {
    int  clue_number;
    char location[128];
    char hint[256];
    struct Clue *next;
};

struct Clue *clue_new(int num, char *loc, char *hint) {
    struct Clue *c = (struct Clue *)malloc(sizeof(struct Clue));
    if (c == NULL) return NULL;
    c->clue_number = num;
    strcpy(c->location, loc);
    strcpy(c->hint, hint);
    c->next = NULL;
    return c;
}

void clue_append(struct Clue **head, struct Clue *c) {
    if (*head == NULL) {
        *head = c;
        return;
    }
    struct Clue *cur = *head;
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = c;
}

void clue_print_all(struct Clue *head) {
    printf("\n--- Treasure Hunt Clues ---\n");
    if (head == NULL) { printf("(no clues)\n"); return; }
    struct Clue *cur = head;
    while (cur != NULL) {
        printf("Clue %d @ %s : %s\n", cur->clue_number, cur->location, cur->hint);
        cur = cur->next;
    }
}

void clue_follow(struct Clue *head) {
    if (head == NULL) { printf("No clues to follow.\n"); return; }
    printf("\nStarting treasure hunt!\n");
    struct Clue *cur = head;
    while (cur != NULL) {
        printf("Go to: %s\n", cur->location);
        printf("Hint : %s\n", cur->hint);
        if (cur->next != NULL) {
            printf("Press Enter to continue...\n");
            getchar();
        } else {
            printf("You found the TREASURE!\n");
        }
        cur = cur->next;
    }
}

void clue_remove_last(struct Clue **head) {
    if (*head == NULL) return;
    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }
    struct Clue *cur = *head;
    while (cur->next->next != NULL)
        cur = cur->next;
    free(cur->next);
    cur->next = NULL;
}

void clue_free(struct Clue *head) {
    while (head != NULL) {
        struct Clue *next = head->next;
        free(head);
        head = next;
    }
}

int main() {
    struct Clue *head = NULL;
    int choice;
    int clue_counter = 1;

    printf("=== Treasure Hunt (Singly Linked List) ===\n");

    while (1) {
        printf("\n1. Add clue\n");
        printf("2. Show all clues\n");
        printf("3. Follow treasure hunt\n");
        printf("4. Remove last clue\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char loc[128], hint[256];
            printf("Location : "); fgets(loc,  sizeof(loc),  stdin); loc[strcspn(loc, "\n")] = 0;
            printf("Hint     : "); fgets(hint, sizeof(hint), stdin); hint[strcspn(hint, "\n")] = 0;
            struct Clue *c = clue_new(clue_counter++, loc, hint);
            if (c == NULL) { printf("Memory error.\n"); continue; }
            clue_append(&head, c);
            printf("Clue added.\n");

        } else if (choice == 2) {
            clue_print_all(head);

        } else if (choice == 3) {
            clue_follow(head);

        } else if (choice == 4) {
            clue_remove_last(&head);
            clue_counter--;
            printf("Last clue removed.\n");

        } else {
            printf("Invalid choice.\n");
        }
    }

    clue_free(head);
    return 0;
}
