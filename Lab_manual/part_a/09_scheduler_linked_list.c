#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Task {
    int  id;
    char name[64];
    int  burst_time;
    struct Task *next;
};

void enqueue(struct Task **head, struct Task **tail, struct Task *t) {
    if (*head == NULL) {
        *head = t;
        *tail = t;
    } else {
        (*tail)->next = t;
        *tail = t;
    }
}

void print_tasks(struct Task *head) {
    printf("\n--- Ready Queue ---\n");
    if (head == NULL) { printf("(empty)\n"); return; }
    struct Task *t = head;
    while (t != NULL) {
        printf("Task %d (%s) : burst = %d ms\n", t->id, t->name, t->burst_time);
        t = t->next;
    }
}

void simulate_fcfs(struct Task *head) {
    printf("\n--- FCFS Schedule ---\n");
    if (head == NULL) { printf("No tasks.\n"); return; }
    int time = 0;
    struct Task *t = head;
    while (t != NULL) {
        printf("t=%3d to t=%3d : Task %d (%s)\n",
               time, time + t->burst_time, t->id, t->name);
        time += t->burst_time;
        t = t->next;
    }
    printf("Total time: %d ms\n", time);
}

void free_tasks(struct Task *head) {
    while (head != NULL) {
        struct Task *next = head->next;
        free(head);
        head = next;
    }
}

int main() {
    struct Task *head = NULL;
    struct Task *tail = NULL;
    int next_id = 1;
    int choice;

    printf("=== CPU Scheduler using Linked List (FCFS) ===\n");

    while (1) {
        printf("\n1. Add task\n");
        printf("2. Show ready queue\n");
        printf("3. Simulate FCFS\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char name[64];
            int burst;
            printf("Task name    : "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
            printf("Burst time (ms): "); scanf("%d", &burst); getchar();
            if (burst < 0) { printf("Burst must be >= 0.\n"); continue; }

            struct Task *t = (struct Task *)malloc(sizeof(struct Task));
            if (t == NULL) { printf("Memory error.\n"); continue; }
            t->id         = next_id++;
            t->burst_time = burst;
            t->next       = NULL;
            strcpy(t->name, name);

            enqueue(&head, &tail, t);
            printf("Task added.\n");

        } else if (choice == 2) {
            print_tasks(head);

        } else if (choice == 3) {
            simulate_fcfs(head);

        } else {
            printf("Invalid choice.\n");
        }
    }

    free_tasks(head);
    return 0;
}
