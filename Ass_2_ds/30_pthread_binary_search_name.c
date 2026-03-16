#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100

typedef struct {
    char (*names)[MAX_NAME];
    int count;
    char target[MAX_NAME];
    int result;
} SearchArgs;

static int compare_names(const void *a, const void *b) {
    const char *left = (const char *) a;
    const char *right = (const char *) b;
    return strcmp(left, right);
}

static void *binary_search_thread(void *arg) {
    SearchArgs *args = (SearchArgs *) arg;
    int left = 0;
    int right = args->count - 1;
    args->result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int compare = strcmp(args->target, args->names[mid]);

        if (compare == 0) {
            args->result = mid;
            break;
        }
        if (compare < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return NULL;
}

int main(void) {
    int n;
    pthread_t thread;
    SearchArgs args = {NULL, 0, "", -1};

    printf("Enter number of names: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0) {
        printf("Invalid number of names.\n");
        return 0;
    }

    args.names = malloc((size_t) n * sizeof(*args.names));
    if (args.names == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }
    args.count = n;

    printf("Enter %d names:\n", n);
    for (int i = 0; i < n; ++i) {
        fgets(args.names[i], MAX_NAME, stdin);
        args.names[i][strcspn(args.names[i], "\n")] = '\0';
    }

    qsort(args.names, (size_t) n, sizeof(*args.names), compare_names);

    printf("Enter name to search: ");
    fgets(args.target, sizeof(args.target), stdin);
    args.target[strcspn(args.target, "\n")] = '\0';

    if (pthread_create(&thread, NULL, binary_search_thread, &args) != 0) {
        printf("Unable to create thread.\n");
        free(args.names);
        return 0;
    }

    pthread_join(thread, NULL);

    printf("\nSorted names:\n");
    for (int i = 0; i < n; ++i) {
        printf("%d. %s\n", i, args.names[i]);
    }

    if (args.result >= 0) {
        printf("Name found at sorted index %d\n", args.result);
    } else {
        printf("Name not found.\n");
    }

    free(args.names);
    return 0;
}
