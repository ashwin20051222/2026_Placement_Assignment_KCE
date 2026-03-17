#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    char name[128];
    struct Person *next;
};

struct Person *person_new(char *name) {
    struct Person *p = (struct Person *)malloc(sizeof(struct Person));
    if (p == NULL) return NULL;
    strcpy(p->name, name);
    p->next = NULL;
    return p;
}

void conga_append(struct Person **tail, struct Person *p) {
    if (*tail == NULL) {

        p->next = p;
        *tail = p;
    } else {
        p->next = (*tail)->next;
        (*tail)->next = p;
        *tail = p;
    }
}

struct Person *conga_find(struct Person *tail, char *name) {
    if (tail == NULL) return NULL;
    struct Person *cur = tail->next;
    do {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    } while (cur != tail->next);
    return NULL;
}

void conga_remove(struct Person **tail, char *name) {
    if (*tail == NULL) return;

    struct Person *prev = *tail;
    struct Person *cur  = (*tail)->next;

    do {
        if (strcmp(cur->name, name) == 0) {
            if (cur == prev) {

                free(cur);
                *tail = NULL;
                return;
            }
            prev->next = cur->next;
            if (cur == *tail) *tail = prev;
            free(cur);
            return;
        }
        prev = cur;
        cur  = cur->next;
    } while (cur != (*tail)->next);
}

void conga_print(struct Person *tail) {
    printf("\n--- Conga Line ---\n");
    if (tail == NULL) { printf("(empty)\n"); return; }
    struct Person *cur = tail->next;
    int idx = 1;
    do {
        printf("%2d. %s\n", idx++, cur->name);
        cur = cur->next;
    } while (cur != tail->next);
}

void conga_free(struct Person **tail) {
    if (*tail == NULL) return;
    struct Person *head = (*tail)->next;
    (*tail)->next = NULL;
    while (head != NULL) {
        struct Person *next = head->next;
        free(head);
        head = next;
    }
    *tail = NULL;
}

int main() {
    struct Person *tail = NULL;
    int choice;

    printf("=== Conga Line (Circular Linked List) ===\n");

    while (1) {
        printf("\n1. Join line\n");
        printf("2. Remove person\n");
        printf("3. Search person\n");
        printf("4. Show line\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char name[128];
            printf("Name: "); fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            struct Person *p = person_new(name);
            if (p == NULL) { printf("Memory error.\n"); continue; }
            conga_append(&tail, p);
            printf("Joined.\n");

        } else if (choice == 2) {
            char name[128];
            printf("Name to remove: "); fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if (conga_find(tail, name) == NULL) { printf("Not found.\n"); continue; }
            conga_remove(&tail, name);
            printf("Removed.\n");

        } else if (choice == 3) {
            char name[128];
            printf("Name to search: "); fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            printf(conga_find(tail, name) ? "Found.\n" : "Not found.\n");

        } else if (choice == 4) {
            conga_print(tail);

        } else {
            printf("Invalid choice.\n");
        }
    }

    conga_free(&tail);
    return 0;
}
