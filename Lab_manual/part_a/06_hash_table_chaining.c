#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TABLE_SIZE = 11
};

struct Entry {
    char key[128];
    int  value;
    struct Entry *next;
};

struct Entry *table[TABLE_SIZE];

int hash_key(char *key) {
    int sum = 0;
    int i;
    for (i = 0; key[i] != '\0'; i++)
        sum += (int)key[i];
    return sum % TABLE_SIZE;
}

void insert(char *key, int value) {
    int idx = hash_key(key);

    struct Entry *e = table[idx];
    while (e != NULL) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return;
        }
        e = e->next;
    }

    struct Entry *new_entry = (struct Entry *)malloc(sizeof(struct Entry));
    if (new_entry == NULL) { printf("Memory error.\n"); return; }
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next  = table[idx];
    table[idx] = new_entry;
}

void search(char *key) {
    int idx = hash_key(key);
    struct Entry *e = table[idx];
    while (e != NULL) {
        if (strcmp(e->key, key) == 0) {
            printf("Found: %s = %d (bucket %d)\n", e->key, e->value, idx);
            return;
        }
        e = e->next;
    }
    printf("Key '%s' not found.\n", key);
}

void delete_key(char *key) {
    int idx = hash_key(key);
    struct Entry *prev = NULL;
    struct Entry *e    = table[idx];
    while (e != NULL) {
        if (strcmp(e->key, key) == 0) {
            if (prev == NULL)
                table[idx] = e->next;
            else
                prev->next = e->next;
            free(e);
            printf("Deleted '%s'.\n", key);
            return;
        }
        prev = e;
        e    = e->next;
    }
    printf("Key '%s' not found.\n", key);
}

void display_table() {
    int i;
    printf("\n--- Hash Table ---\n");
    for (i = 0; i < TABLE_SIZE; i++) {
        printf("[%2d]: ", i);
        struct Entry *e = table[i];
        if (e == NULL) {
            printf("(empty)");
        }
        while (e != NULL) {
            printf("(%s=%d)", e->key, e->value);
            if (e->next != NULL) printf(" -> ");
            e = e->next;
        }
        printf("\n");
    }
}

void free_table() {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        struct Entry *e = table[i];
        while (e != NULL) {
            struct Entry *next = e->next;
            free(e);
            e = next;
        }
        table[i] = NULL;
    }
}

int main() {
    int choice;

    int i;
    for (i = 0; i < TABLE_SIZE; i++) table[i] = NULL;

    printf("=== Hash Table using Linked List Chaining ===\n");

    while (1) {
        printf("\n1. Insert / Update\n");
        printf("2. Search\n");
        printf("3. Delete\n");
        printf("4. Display table\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char key[128]; int value;
            printf("Key   : "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
            printf("Value : "); scanf("%d", &value); getchar();
            insert(key, value);
            printf("Done.\n");

        } else if (choice == 2) {
            char key[128];
            printf("Key to search: "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
            search(key);

        } else if (choice == 3) {
            char key[128];
            printf("Key to delete: "); fgets(key, sizeof(key), stdin); key[strcspn(key, "\n")] = 0;
            delete_key(key);

        } else if (choice == 4) {
            display_table();

        } else {
            printf("Invalid choice.\n");
        }
    }

    free_table();
    return 0;
}
