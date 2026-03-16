#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 13

typedef struct Entry {
    char key[100];
    int value;
    struct Entry *next;
} Entry;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static unsigned int hash_key(const char *key) {
    unsigned int hash = 0;
    while (*key != '\0') {
        hash = hash * 31u + (unsigned char) *key++;
    }
    return hash % TABLE_SIZE;
}

static void insert_entry(Entry *table[], const char *key, int value) {
    unsigned int index = hash_key(key);
    Entry *temp = table[index];

    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            temp->value = value;
            printf("Updated existing key.\n");
            return;
        }
        temp = temp->next;
    }

    Entry *entry = (Entry *) malloc(sizeof(Entry));
    if (entry == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(entry->key, key, sizeof(entry->key) - 1);
    entry->key[sizeof(entry->key) - 1] = '\0';
    entry->value = value;
    entry->next = table[index];
    table[index] = entry;
}

static void search_entry(Entry *table[], const char *key) {
    unsigned int index = hash_key(key);
    Entry *temp = table[index];

    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            printf("Key found. Value = %d\n", temp->value);
            return;
        }
        temp = temp->next;
    }

    printf("Key not found.\n");
}

static void delete_entry(Entry *table[], const char *key) {
    unsigned int index = hash_key(key);
    Entry *temp = table[index];
    Entry *prev = NULL;

    while (temp != NULL && strcmp(temp->key, key) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Key not found.\n");
        return;
    }

    if (prev == NULL) {
        table[index] = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
    printf("Key deleted.\n");
}

static void display_table(Entry *table[]) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        printf("[%d] ", i);
        Entry *temp = table[i];
        while (temp != NULL) {
            printf("(%s, %d) -> ", temp->key, temp->value);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

static void free_table(Entry *table[]) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Entry *temp = table[i];
        while (temp != NULL) {
            Entry *next = temp->next;
            free(temp);
            temp = next;
        }
    }
}

int main(void) {
    Entry *table[TABLE_SIZE] = {0};
    int choice;
    int value;
    char key[100];

    while (1) {
        printf("\nHash Table Menu\n");
        printf("1. Insert or update key\n");
        printf("2. Search key\n");
        printf("3. Delete key\n");
        printf("4. Display table\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter key: ");
                read_line(key, sizeof(key));
                printf("Enter integer value: ");
                scanf("%d", &value);
                getchar();
                insert_entry(table, key, value);
                break;
            case 2:
                printf("Enter key to search: ");
                read_line(key, sizeof(key));
                search_entry(table, key);
                break;
            case 3:
                printf("Enter key to delete: ");
                read_line(key, sizeof(key));
                delete_entry(table, key);
                break;
            case 4:
                display_table(table);
                break;
            case 5:
                free_table(table);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_table(table);
    return 0;
}
