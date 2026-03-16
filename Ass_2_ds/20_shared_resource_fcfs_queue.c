#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Request {
    char process_name[100];
    int requested_time;
    struct Request *next;
} Request;

typedef struct {
    Request *front;
    Request *rear;
} Queue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Request *create_request(const char *name, int time) {
    Request *request = (Request *) malloc(sizeof(Request));
    if (request == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(request->process_name, name, sizeof(request->process_name) - 1);
    request->process_name[sizeof(request->process_name) - 1] = '\0';
    request->requested_time = time;
    request->next = NULL;
    return request;
}

static void enqueue(Queue *queue, Request *request) {
    if (queue->rear == NULL) {
        queue->front = queue->rear = request;
    } else {
        queue->rear->next = request;
        queue->rear = request;
    }
}

static Request *dequeue(Queue *queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    Request *request = queue->front;
    queue->front = request->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    request->next = NULL;
    return request;
}

static void display_queue(const Queue *queue, const Request *current) {
    if (current != NULL) {
        printf("Resource currently allocated to %s for %d unit(s)\n",
               current->process_name,
               current->requested_time);
    } else {
        printf("Resource is currently free.\n");
    }

    if (queue->front == NULL) {
        printf("No waiting requests.\n");
        return;
    }

    printf("Waiting queue:\n");
    Request *temp = queue->front;
    while (temp != NULL) {
        printf("Process: %-15s Time: %d\n", temp->process_name, temp->requested_time);
        temp = temp->next;
    }
}

static void free_queue(Queue *queue) {
    while (queue->front != NULL) {
        Request *next = queue->front->next;
        free(queue->front);
        queue->front = next;
    }
    queue->rear = NULL;
}

int main(void) {
    Queue queue = {NULL, NULL};
    Request *current = NULL;
    int choice;
    int time;
    char name[100];

    while (1) {
        printf("\nShared Resource FCFS Menu\n");
        printf("1. Request resource\n");
        printf("2. Release current resource holder\n");
        printf("3. Display allocation status\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter process name: ");
                read_line(name, sizeof(name));
                printf("Enter requested time units: ");
                scanf("%d", &time);
                getchar();
                if (current == NULL) {
                    current = create_request(name, time);
                    printf("Resource allocated immediately to %s\n", current->process_name);
                } else {
                    enqueue(&queue, create_request(name, time));
                    printf("Resource busy. Request added to waiting queue.\n");
                }
                break;
            case 2:
                if (current == NULL) {
                    printf("No process is holding the resource.\n");
                } else {
                    printf("Releasing resource from %s\n", current->process_name);
                    free(current);
                    current = dequeue(&queue);
                    if (current != NULL) {
                        printf("Resource allocated next to %s\n", current->process_name);
                    }
                }
                break;
            case 3:
                display_queue(&queue, current);
                break;
            case 4:
                free(current);
                free_queue(&queue);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free(current);
    free_queue(&queue);
    return 0;
}
