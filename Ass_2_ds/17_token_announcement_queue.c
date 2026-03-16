#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TokenNode {
    int token_number;
    char counter_name[100];
    struct TokenNode *next;
} TokenNode;

typedef struct {
    TokenNode *front;
    TokenNode *rear;
    int next_token;
} TokenQueue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static void enqueue(TokenQueue *queue, const char *counter_name) {
    TokenNode *node = (TokenNode *) malloc(sizeof(TokenNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->token_number = queue->next_token++;
    strncpy(node->counter_name, counter_name, sizeof(node->counter_name) - 1);
    node->counter_name[sizeof(node->counter_name) - 1] = '\0';
    node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }

    printf("Issued token %d for %s\n", node->token_number, node->counter_name);
}

static void dequeue(TokenQueue *queue) {
    if (queue->front == NULL) {
        printf("No tokens waiting.\n");
        return;
    }

    TokenNode *temp = queue->front;
    printf("Now serving token %d at %s\n", temp->token_number, temp->counter_name);
    queue->front = temp->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
}

static void display_queue(const TokenQueue *queue) {
    if (queue->front == NULL) {
        printf("No tokens in queue.\n");
        return;
    }

    printf("\nWaiting tokens:\n");
    TokenNode *temp = queue->front;
    while (temp != NULL) {
        printf("Token %d -> %s\n", temp->token_number, temp->counter_name);
        temp = temp->next;
    }
}

static void free_queue(TokenQueue *queue) {
    while (queue->front != NULL) {
        TokenNode *next = queue->front->next;
        free(queue->front);
        queue->front = next;
    }
    queue->rear = NULL;
}

int main(void) {
    TokenQueue queue = {NULL, NULL, 1};
    int choice;
    char counter_name[100];

    while (1) {
        printf("\nToken Announcement Queue Menu\n");
        printf("1. Generate token\n");
        printf("2. Announce next token\n");
        printf("3. Display waiting queue\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter service counter name (Bank/Hospital desk): ");
                read_line(counter_name, sizeof(counter_name));
                enqueue(&queue, counter_name);
                break;
            case 2:
                dequeue(&queue);
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
