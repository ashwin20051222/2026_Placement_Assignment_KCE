#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char text[200];
    struct Message *next;
} Message;

typedef struct {
    Message *front;
    Message *rear;
} Queue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Message *create_message(const char *text) {
    Message *message = (Message *) malloc(sizeof(Message));
    if (message == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(message->text, text, sizeof(message->text) - 1);
    message->text[sizeof(message->text) - 1] = '\0';
    message->next = NULL;
    return message;
}

static void enqueue(Queue *queue, Message *message) {
    if (queue->rear == NULL) {
        queue->front = queue->rear = message;
    } else {
        queue->rear->next = message;
        queue->rear = message;
    }
}

static Message *dequeue(Queue *queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    Message *message = queue->front;
    queue->front = message->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    message->next = NULL;
    return message;
}

static void display_queue(const char *label, const Queue *queue) {
    printf("%s:\n", label);
    if (queue->front == NULL) {
        printf("  Empty\n");
        return;
    }

    Message *temp = queue->front;
    while (temp != NULL) {
        printf("  %s\n", temp->text);
        temp = temp->next;
    }
}

static void free_queue(Queue *queue) {
    while (queue->front != NULL) {
        Message *next = queue->front->next;
        free(queue->front);
        queue->front = next;
    }
    queue->rear = NULL;
}

int main(void) {
    Queue message_queue = {NULL, NULL};
    Queue pipe_buffer = {NULL, NULL};
    int choice;
    char text[200];

    while (1) {
        printf("\nMessage Queue and Pipe Buffer Menu\n");
        printf("1. Send message to message queue\n");
        printf("2. Move one message to pipe buffer\n");
        printf("3. Receive message from pipe buffer\n");
        printf("4. Display queues\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter message text: ");
                read_line(text, sizeof(text));
                enqueue(&message_queue, create_message(text));
                break;
            case 2:
                {
                    Message *message = dequeue(&message_queue);
                    if (message == NULL) {
                        printf("Message queue is empty.\n");
                    } else {
                        enqueue(&pipe_buffer, message);
                        printf("Message moved into pipe buffer.\n");
                    }
                }
                break;
            case 3:
                {
                    Message *message = dequeue(&pipe_buffer);
                    if (message == NULL) {
                        printf("Pipe buffer is empty.\n");
                    } else {
                        printf("Consumer received: %s\n", message->text);
                        free(message);
                    }
                }
                break;
            case 4:
                display_queue("Message queue", &message_queue);
                display_queue("Pipe buffer", &pipe_buffer);
                break;
            case 5:
                free_queue(&message_queue);
                free_queue(&pipe_buffer);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_queue(&message_queue);
    free_queue(&pipe_buffer);
    return 0;
}
