#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Clue {
    char text[200];
    int completed;
    struct Clue *next;
} Clue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Clue *create_clue(const char *text) {
    Clue *clue = (Clue *) malloc(sizeof(Clue));
    if (clue == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(clue->text, text, sizeof(clue->text) - 1);
    clue->text[sizeof(clue->text) - 1] = '\0';
    clue->completed = 0;
    clue->next = NULL;
    return clue;
}

static void add_clue(Clue **head, const char *text) {
    Clue *clue = create_clue(text);
    if (*head == NULL) {
        *head = clue;
        return;
    }

    Clue *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = clue;
}

static void mark_next_completed(Clue *head) {
    while (head != NULL && head->completed) {
        head = head->next;
    }

    if (head == NULL) {
        printf("All clues are already completed.\n");
        return;
    }

    head->completed = 1;
    printf("Completed clue: %s\n", head->text);
}

static void display_clues(Clue *head) {
    if (head == NULL) {
        printf("No clues available.\n");
        return;
    }

    int index = 1;
    while (head != NULL) {
        printf("%d. [%s] %s\n", index++, head->completed ? "Done" : "Pending", head->text);
        head = head->next;
    }
}

static void find_clue(Clue *head, const char *keyword) {
    int found = 0;
    while (head != NULL) {
        if (strstr(head->text, keyword) != NULL) {
            printf("Found clue: %s\n", head->text);
            found = 1;
        }
        head = head->next;
    }

    if (!found) {
        printf("No clue matched the keyword.\n");
    }
}

static void free_clues(Clue *head) {
    while (head != NULL) {
        Clue *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    Clue *head = NULL;
    int choice;
    char text[200];

    while (1) {
        printf("\nScavenger Hunt Menu\n");
        printf("1. Add clue\n");
        printf("2. Mark next clue as completed\n");
        printf("3. Display all clues\n");
        printf("4. Search clue by keyword\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter clue text: ");
                read_line(text, sizeof(text));
                add_clue(&head, text);
                break;
            case 2:
                mark_next_completed(head);
                break;
            case 3:
                display_clues(head);
                break;
            case 4:
                printf("Enter keyword: ");
                read_line(text, sizeof(text));
                find_clue(head, text);
                break;
            case 5:
                free_clues(head);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_clues(head);
    return 0;
}
