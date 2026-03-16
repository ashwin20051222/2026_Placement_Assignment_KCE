#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char name[100];
    struct Person *next;
} Person;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Person *create_person(const char *name) {
    Person *person = (Person *) malloc(sizeof(Person));
    if (person == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(person->name, name, sizeof(person->name) - 1);
    person->name[sizeof(person->name) - 1] = '\0';
    person->next = NULL;
    return person;
}

static void join_end(Person **head, const char *name) {
    Person *person = create_person(name);
    if (*head == NULL) {
        *head = person;
        return;
    }

    Person *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = person;
}

static void join_front(Person **head, const char *name) {
    Person *person = create_person(name);
    person->next = *head;
    *head = person;
}

static void remove_person(Person **head, const char *name) {
    Person *temp = *head;
    Person *prev = NULL;

    while (temp != NULL && strcmp(temp->name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Person not found in the congo line.\n");
        return;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
}

static void display_line(Person *head) {
    if (head == NULL) {
        printf("The congo line is empty.\n");
        return;
    }

    printf("\nCongo line order:\n");
    while (head != NULL) {
        printf("%s", head->name);
        head = head->next;
        if (head != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static void free_line(Person *head) {
    while (head != NULL) {
        Person *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    Person *head = NULL;
    int choice;
    char name[100];

    while (1) {
        printf("\nCongo Line Menu\n");
        printf("1. Join at end\n");
        printf("2. Join at front\n");
        printf("3. Remove person\n");
        printf("4. Display line\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter person name: ");
                read_line(name, sizeof(name));
                join_end(&head, name);
                break;
            case 2:
                printf("Enter person name: ");
                read_line(name, sizeof(name));
                join_front(&head, name);
                break;
            case 3:
                printf("Enter name to remove: ");
                read_line(name, sizeof(name));
                remove_person(&head, name);
                break;
            case 4:
                display_line(head);
                break;
            case 5:
                free_line(head);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_line(head);
    return 0;
}
