#include <stdio.h>
#include <stdlib.h>

typedef struct Player {
    int id;
    struct Player *next;
} Player;

static Player *create_circle(int n) {
    Player *head = NULL;
    Player *tail = NULL;

    for (int i = 1; i <= n; ++i) {
        Player *node = (Player *) malloc(sizeof(Player));
        if (node == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        node->id = i;
        node->next = NULL;

        if (head == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    if (tail != NULL) {
        tail->next = head;
    }
    return tail;
}

int main(void) {
    int n;
    int k;

    printf("Enter number of players: ");
    scanf("%d", &n);
    printf("Enter elimination step k: ");
    scanf("%d", &k);

    if (n <= 0 || k <= 0) {
        printf("N and k must be positive.\n");
        return 0;
    }

    Player *tail = create_circle(n);
    Player *prev = tail;

    printf("Elimination order: ");
    while (prev->next != prev) {
        for (int step = 1; step < k; ++step) {
            prev = prev->next;
        }

        Player *removed = prev->next;
        printf("%d ", removed->id);
        prev->next = removed->next;
        if (removed == tail) {
            tail = prev;
        }
        free(removed);
    }

    printf("\nLast remaining player: %d\n", prev->id);
    free(prev);
    return 0;
}
