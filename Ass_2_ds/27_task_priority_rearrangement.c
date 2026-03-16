#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char name[100];
    int priority;
    struct Task *next;
} Task;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Task *create_task(const char *name, int priority) {
    Task *task = (Task *) malloc(sizeof(Task));
    if (task == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0';
    task->priority = priority;
    task->next = NULL;
    return task;
}

static void insert_by_priority(Task **head, const char *name, int priority) {
    Task *task = create_task(name, priority);

    if (*head == NULL || priority > (*head)->priority) {
        task->next = *head;
        *head = task;
        return;
    }

    Task *temp = *head;
    while (temp->next != NULL && temp->next->priority >= priority) {
        temp = temp->next;
    }
    task->next = temp->next;
    temp->next = task;
}

static void display_tasks(Task *head) {
    if (head == NULL) {
        printf("No tasks available.\n");
        return;
    }

    printf("\nTask list rearranged by priority:\n");
    while (head != NULL) {
        printf("Task: %-15s Priority: %d\n", head->name, head->priority);
        head = head->next;
    }
}

static void free_tasks(Task *head) {
    while (head != NULL) {
        Task *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    Task *head = NULL;
    int choice;
    int priority;
    char name[100];

    while (1) {
        printf("\nTask Priority Menu\n");
        printf("1. Add task\n");
        printf("2. Display rearranged task list\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter task name: ");
                read_line(name, sizeof(name));
                printf("Enter task priority: ");
                scanf("%d", &priority);
                getchar();
                insert_by_priority(&head, name, priority);
                break;
            case 2:
                display_tasks(head);
                break;
            case 3:
                free_tasks(head);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_tasks(head);
    return 0;
}
