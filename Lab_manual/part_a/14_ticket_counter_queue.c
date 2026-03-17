#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Customer {
    int  token_number;
    char name[64];
    int  service_time;
    struct Customer *next;
};

struct Customer *front = NULL;
struct Customer *rear  = NULL;
int token_counter = 1;

void enqueue(char *name, int service_time) {
    struct Customer *c = (struct Customer *)malloc(sizeof(struct Customer));
    if (c == NULL) { printf("Memory error.\n"); return; }
    c->token_number = token_counter++;
    c->service_time = service_time;
    c->next         = NULL;
    strcpy(c->name, name);

    if (rear == NULL) {
        front = c;
        rear  = c;
    } else {
        rear->next = c;
        rear       = c;
    }
    printf("Token #%d issued to %s.\n", c->token_number, c->name);
}

void serve_next() {
    if (front == NULL) { printf("No customers waiting.\n"); return; }
    struct Customer *served = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    printf("Serving Token #%d - %s (service time: %d min)\n",
           served->token_number, served->name, served->service_time);
    free(served);
}

void print_queue() {
    printf("\n--- Waiting Queue ---\n");
    if (front == NULL) { printf("(no one waiting)\n"); return; }
    struct Customer *cur = front;
    int pos = 1;
    int wait = 0;
    while (cur != NULL) {
        printf("Position %d: Token #%d - %-15s (service: %d min, wait: ~%d min)\n",
               pos, cur->token_number, cur->name, cur->service_time, wait);
        wait += cur->service_time;
        pos++;
        cur = cur->next;
    }
}

void free_queue() {
    while (front != NULL) {
        struct Customer *next = front->next;
        free(front);
        front = next;
    }
    rear = NULL;
}

int main() {
    int choice;
    printf("=== Ticket Counter Simulation (Queue) ===\n");

    while (1) {
        printf("\n1. Customer arrives (join queue)\n");
        printf("2. Serve next customer\n");
        printf("3. Show queue\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char name[64];
            int svc;
            printf("Customer name       : "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
            printf("Service time (min)  : "); scanf("%d", &svc); getchar();
            enqueue(name, svc);

        } else if (choice == 2) {
            serve_next();

        } else if (choice == 3) {
            print_queue();

        } else {
            printf("Invalid choice.\n");
        }
    }

    free_queue();
    return 0;
}
