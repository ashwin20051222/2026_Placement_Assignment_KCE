#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Customer {
    char name[100];
    int tickets;
    struct Customer *next;
} Customer;

typedef struct {
    Customer *front;
    Customer *rear;
} Queue;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static void enqueue(Queue *queue, const char *name, int tickets) {
    Customer *customer = (Customer *) malloc(sizeof(Customer));
    if (customer == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(customer->name, name, sizeof(customer->name) - 1);
    customer->name[sizeof(customer->name) - 1] = '\0';
    customer->tickets = tickets;
    customer->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = customer;
    } else {
        queue->rear->next = customer;
        queue->rear = customer;
    }
}

static void serve_customer(Queue *queue) {
    if (queue->front == NULL) {
        printf("No reservation requests pending.\n");
        return;
    }

    Customer *customer = queue->front;
    printf("Booking %d ticket(s) for %s\n", customer->tickets, customer->name);
    queue->front = customer->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(customer);
}

static void display_queue(const Queue *queue) {
    if (queue->front == NULL) {
        printf("Ticket queue is empty.\n");
        return;
    }

    Customer *temp = queue->front;
    while (temp != NULL) {
        printf("Customer: %-15s Tickets: %d\n", temp->name, temp->tickets);
        temp = temp->next;
    }
}

static void free_queue(Queue *queue) {
    while (queue->front != NULL) {
        Customer *next = queue->front->next;
        free(queue->front);
        queue->front = next;
    }
    queue->rear = NULL;
}

int main(void) {
    Queue queue = {NULL, NULL};
    int choice;
    int tickets;
    char name[100];

    while (1) {
        printf("\nTicket Counter Menu\n");
        printf("1. Add reservation request\n");
        printf("2. Serve next customer\n");
        printf("3. Display queue\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter customer name: ");
                read_line(name, sizeof(name));
                printf("Enter number of tickets: ");
                scanf("%d", &tickets);
                getchar();
                enqueue(&queue, name, tickets);
                break;
            case 2:
                serve_customer(&queue);
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
