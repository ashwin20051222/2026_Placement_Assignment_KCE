#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_PROC = 50
};

struct Process {
    int  pid;
    char name[64];
    int  priority;
    int  burst_time;
};

struct Process procs[MAX_PROC];
int num_procs = 0;

void swap(int i, int j) {
    struct Process tmp = procs[i];
    procs[i] = procs[j];
    procs[j] = tmp;
}

void heapify(int n, int root) {
    int largest = root;
    int left    = 2 * root + 1;
    int right   = 2 * root + 2;

    if (left  < n && procs[left].priority  > procs[largest].priority) largest = left;
    if (right < n && procs[right].priority > procs[largest].priority) largest = right;
    if (largest != root) {
        swap(root, largest);
        heapify(n, largest);
    }
}

void heap_sort() {
    int i;

    for (i = num_procs / 2 - 1; i >= 0; i--)
        heapify(num_procs, i);

    for (i = num_procs - 1; i > 0; i--) {
        swap(0, i);
        heapify(i, 0);
    }

}

void print_procs() {
    int i;
    printf("\n--- Process List ---\n");
    if (num_procs == 0) { printf("(no processes)\n"); return; }
    printf("  PID  %-15s  Priority  Burst\n", "Name");
    printf("  ---  %-15s  --------  -----\n", "---------------");
    for (i = 0; i < num_procs; i++)
        printf("  %3d  %-15s  %8d  %5d ms\n",
               procs[i].pid, procs[i].name, procs[i].priority, procs[i].burst_time);
}

void simulate() {
    if (num_procs == 0) { printf("No processes.\n"); return; }
    heap_sort();
    printf("\n--- Priority Schedule (Heap Sort) ---\n");
    int time = 0;
    int i;
    for (i = 0; i < num_procs; i++) {
        printf("t=%3d to t=%3d : PID %d (%s) [priority=%d]\n",
               time, time + procs[i].burst_time,
               procs[i].pid, procs[i].name, procs[i].priority);
        time += procs[i].burst_time;
    }
    printf("Total time: %d ms\n", time);
}

int main() {
    int choice, pid_counter = 1;
    printf("=== Priority Scheduling using Heap Sort ===\n");

    while (1) {
        printf("\n1. Add process\n");
        printf("2. Show processes\n");
        printf("3. Run priority schedule\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            if (num_procs >= MAX_PROC) { printf("Max processes reached.\n"); continue; }
            char name[64]; int prio, burst;
            printf("Process name  : "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
            printf("Priority (1=highest): "); scanf("%d", &prio);  getchar();
            printf("Burst time (ms): ");      scanf("%d", &burst); getchar();
            procs[num_procs].pid        = pid_counter++;
            procs[num_procs].priority   = prio;
            procs[num_procs].burst_time = burst;
            strcpy(procs[num_procs].name, name);
            num_procs++;
            printf("Process added.\n");

        } else if (choice == 2) {
            print_procs();

        } else if (choice == 3) {
            simulate();

        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}
