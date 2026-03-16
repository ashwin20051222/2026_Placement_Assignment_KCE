#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char name[100];
    int duration;
    struct Task *next;
} Task;

typedef struct {
    Task *front;
    Task *rear;
} Queue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static void enqueue(Queue *queue, const char *name, int duration) {
    Task *task = (Task *) malloc(sizeof(Task));
    if (task == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0';
    task->duration = duration;
    task->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = task;
    } else {
        queue->rear->next = task;
        queue->rear = task;
    }
}

static void process_next(Queue *queue) {
    if (queue->front == NULL) {
        printf("Task queue is empty.\n");
        return;
    }

    Task *task = queue->front;
    printf("Processing task %s for %d unit(s)\n", task->name, task->duration);
    queue->front = task->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(task);
}

static void display_queue(const Queue *queue) {
    if (queue->front == NULL) {
        printf("No tasks in queue.\n");
        return;
    }

    Task *temp = queue->front;
    while (temp != NULL) {
        printf("Task: %-15s Duration: %d\n", temp->name, temp->duration);
        temp = temp->next;
    }
}

static void free_queue(Queue *queue) {
    while (queue->front != NULL) {
        Task *next = queue->front->next;
        free(queue->front);
        queue->front = next;
    }
    queue->rear = NULL;
}

int main(void) {
    Queue queue = {NULL, NULL};
    int choice;
    int duration;
    char name[100];

    while (1) {
        printf("\nTask Queue Menu\n");
        printf("1. Add task\n");
        printf("2. Process next task\n");
        printf("3. Display task queue\n");
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
                printf("Enter task duration: ");
                scanf("%d", &duration);
                getchar();
                enqueue(&queue, name, duration);
                break;
            case 2:
                process_next(&queue);
                break;
            case 3:
                display_queue(&queue);
                break;
            case 4:
                free_queue(&queue);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_queue(&queue);
    return 0;
}
