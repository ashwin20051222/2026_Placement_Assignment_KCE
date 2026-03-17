#include <stdio.h>
#include <stdlib.h>

struct Node {
    int id;
    struct Node *next;
};

struct Node *make_circle(int n) {
    struct Node *head = NULL;
    struct Node *tail = NULL;
    int i;
    for (i = 1; i <= n; i++) {
        struct Node *x = (struct Node *)malloc(sizeof(struct Node));
        if (x == NULL) return NULL;
        x->id   = i;
        x->next = NULL;
        if (head == NULL) {
            head = x;
            tail = x;
        } else {
            tail->next = x;
            tail = x;
        }
    }
    tail->next = head;
    return tail;
}

int main() {
    int n, k;

    printf("=== Josephus Problem (Circular Linked List) ===\n");
    printf("Enter number of players (n): ");
    scanf("%d", &n);
    printf("Enter step count (k): ");
    scanf("%d", &k);

    if (n <= 0 || k <= 0) {
        printf("n and k must be positive.\n");
        return 1;
    }

    struct Node *tail = make_circle(n);
    if (tail == NULL) { printf("Memory error.\n"); return 1; }

    struct Node *prev = tail;
    struct Node *cur  = tail->next;

    printf("Elimination order: ");

    while (cur != cur->next) {

        int step;
        for (step = 1; step < k; step++) {
            prev = cur;
            cur  = cur->next;
        }

        printf("%d ", cur->id);
        prev->next = cur->next;
        if (cur == tail) tail = prev;
        free(cur);
        cur = prev->next;
    }

    printf("\nWinner: %d\n", cur->id);
    free(cur);
    return 0;
}
