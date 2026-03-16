#include <stdio.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    char name[100];
    int priority;
    int burst_time;
} Process;

static void swap(Process *a, Process *b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify(Process processes[], int n, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < n && processes[left].priority > processes[largest].priority) {
        largest = left;
    }
    if (right < n && processes[right].priority > processes[largest].priority) {
        largest = right;
    }

    if (largest != index) {
        swap(&processes[index], &processes[largest]);
        heapify(processes, n, largest);
    }
}

static void heap_sort(Process processes[], int n) {
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(processes, n, i);
    }

    for (int i = n - 1; i > 0; --i) {
        swap(&processes[0], &processes[i]);
        heapify(processes, i, 0);
    }
}

int main(void) {
    Process processes[MAX_PROCESSES];
    int n;

    printf("Enter number of processes: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_PROCESSES) {
        printf("Invalid number of processes.\n");
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        printf("Enter process %d name: ", i + 1);
        fgets(processes[i].name, sizeof(processes[i].name), stdin);
        processes[i].name[strcspn(processes[i].name, "\n")] = '\0';
        printf("Enter priority for %s: ", processes[i].name);
        scanf("%d", &processes[i].priority);
        printf("Enter burst time for %s: ", processes[i].name);
        scanf("%d", &processes[i].burst_time);
        getchar();
    }

    heap_sort(processes, n);

    printf("\nPriority-based scheduling order using heap sort:\n");
    for (int i = n - 1; i >= 0; --i) {
        printf("Process: %-15s Priority: %-4d Burst: %d\n",
               processes[i].name, processes[i].priority, processes[i].burst_time);
    }

    return 0;
}
