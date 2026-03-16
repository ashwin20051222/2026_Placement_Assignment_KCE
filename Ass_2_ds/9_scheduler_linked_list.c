#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char name[100];
    int arrival_time;
    int burst_time;
    struct Task *next;
} Task;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Task *create_task(const char *name, int arrival, int burst) {
    Task *task = (Task *) malloc(sizeof(Task));
    if (task == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0';
    task->arrival_time = arrival;
    task->burst_time = burst;
    task->next = NULL;
    return task;
}

static void schedule_task(Task **head, const char *name, int arrival, int burst) {
    Task *task = create_task(name, arrival, burst);

    if (*head == NULL || arrival < (*head)->arrival_time) {
        task->next = *head;
        *head = task;
        return;
    }

    Task *temp = *head;
    while (temp->next != NULL && temp->next->arrival_time <= arrival) {
        temp = temp->next;
    }
    task->next = temp->next;
    temp->next = task;
}

static void run_next_task(Task **head, int *current_time) {
    if (*head == NULL) {
        printf("No tasks scheduled.\n");
        return;
    }

    Task *task = *head;
    *head = task->next;

    if (*current_time < task->arrival_time) {
        *current_time = task->arrival_time;
    }

    printf("Running task %s from time %d to %d\n",
           task->name, *current_time, *current_time + task->burst_time);
    *current_time += task->burst_time;
    free(task);
}

static void display_tasks(Task *head) {
    if (head == NULL) {
        printf("Scheduler queue is empty.\n");
        return;
    }

    printf("\nPending tasks:\n");
    while (head != NULL) {
        printf("Task: %-15s Arrival: %-4d Burst: %d\n",
               head->name, head->arrival_time, head->burst_time);
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
    int current_time = 0;
    int choice;
    int arrival;
    int burst;
    char name[100];

    while (1) {
        printf("\nScheduler Menu\n");
        printf("1. Add task\n");
        printf("2. Run next task\n");
        printf("3. Display pending tasks\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter task name: ");
                read_line(name, sizeof(name));
                printf("Enter arrival time: ");
                scanf("%d", &arrival);
                printf("Enter burst time: ");
                scanf("%d", &burst);
                getchar();
                schedule_task(&head, name, arrival, burst);
                break;
            case 2:
                run_next_task(&head, &current_time);
                break;
            case 3:
                display_tasks(head);
                break;
            case 4:
                free_tasks(head);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_tasks(head);
    return 0;
}
