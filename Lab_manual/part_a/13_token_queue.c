#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char token[128];
    struct Node *next;
};

struct Node *q_head = NULL;
struct Node *q_tail = NULL;

void enqueue(char *token) {
    struct Node *n = (struct Node *)malloc(sizeof(struct Node));
    if (n == NULL) { printf("Memory error.\n"); return; }
    strcpy(n->token, token);
    n->next = NULL;
    if (q_tail == NULL) {
        q_head = n;
        q_tail = n;
    } else {
        q_tail->next = n;
        q_tail       = n;
    }
}

char *dequeue(char *out_buf) {
    if (q_head == NULL) return NULL;
    struct Node *front = q_head;
    strcpy(out_buf, front->token);
    q_head = front->next;
    if (q_head == NULL) q_tail = NULL;
    free(front);
    return out_buf;
}

void print_queue() {
    printf("\n--- Queue Contents ---\n");
    if (q_head == NULL) { printf("(empty)\n"); return; }
    struct Node *cur = q_head;
    int idx = 1;
    while (cur != NULL) {
        printf("%2d. %s\n", idx++, cur->token);
        cur = cur->next;
    }
}

void free_queue() {
    while (q_head != NULL) {
        struct Node *next = q_head->next;
        free(q_head);
        q_head = next;
    }
    q_tail = NULL;
}

int main() {
    int choice;
    printf("=== Token Queue (FIFO) ===\n");

    while (1) {
        printf("\n1. Tokenize and enqueue a sentence\n");
        printf("2. Dequeue one token\n");
        printf("3. Show queue\n");
        printf("4. Dequeue all tokens\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char sentence[512];
            printf("Enter sentence: ");
            fgets(sentence, sizeof(sentence), stdin);
            sentence[strcspn(sentence, "\n")] = 0;

            char *tok = strtok(sentence, " \t");
            int count = 0;
            while (tok != NULL) {
                enqueue(tok);
                count++;
                tok = strtok(NULL, " \t");
            }
            printf("Enqueued %d tokens.\n", count);

        } else if (choice == 2) {
            char buf[128];
            if (dequeue(buf) != NULL)
                printf("Dequeued: \"%s\"\n", buf);
            else
                printf("Queue is empty.\n");

        } else if (choice == 3) {
            print_queue();

        } else if (choice == 4) {
            char buf[128];
            printf("Dequeuing all:\n");
            while (dequeue(buf) != NULL)
                printf("  %s\n", buf);

        } else {
            printf("Invalid choice.\n");
        }
    }

    free_queue();
    return 0;
}
