#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 1000

typedef struct {
    char title[100];
    char author[100];
    int id;
} Book;

static int compare_books(const void *a, const void *b) {
    const Book *left = (const Book *) a;
    const Book *right = (const Book *) b;
    return strcmp(left->title, right->title);
}

static int binary_search_books(const Book books[], int n, const char *title) {
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int compare = strcmp(title, books[mid].title);

        if (compare == 0) {
            return mid;
        }
        if (compare < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return -1;
}

int main(void) {
    Book books[MAX_BOOKS];
    int n;
    char target[100];

    printf("Enter number of books: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_BOOKS) {
        printf("Invalid number of books.\n");
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        printf("Enter title of book %d: ", i + 1);
        fgets(books[i].title, sizeof(books[i].title), stdin);
        books[i].title[strcspn(books[i].title, "\n")] = '\0';
        printf("Enter author of %s: ", books[i].title);
        fgets(books[i].author, sizeof(books[i].author), stdin);
        books[i].author[strcspn(books[i].author, "\n")] = '\0';
        printf("Enter ID of %s: ", books[i].title);
        scanf("%d", &books[i].id);
        getchar();
    }

    qsort(books, (size_t) n, sizeof(books[0]), compare_books);

    printf("Enter exact book title to search: ");
    fgets(target, sizeof(target), stdin);
    target[strcspn(target, "\n")] = '\0';

    int index = binary_search_books(books, n, target);
    if (index >= 0) {
        printf("Book found.\n");
        printf("Title : %s\n", books[index].title);
        printf("Author: %s\n", books[index].author);
        printf("ID    : %d\n", books[index].id);
    } else {
        printf("Book not found.\n");
    }

    return 0;
}
