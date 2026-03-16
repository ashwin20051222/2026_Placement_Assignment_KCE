#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_NAME 100

static void swap_names(char names[][MAX_NAME], int i, int j) {
    char temp[MAX_NAME];
    strcpy(temp, names[i]);
    strcpy(names[i], names[j]);
    strcpy(names[j], temp);
}

int main(void) {
    int n;
    char names[MAX_ITEMS][MAX_NAME];

    printf("Enter number of names in NAND Flash table: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_ITEMS) {
        printf("Invalid number of entries.\n");
        return 0;
    }

    printf("Enter %d names:\n", n);
    for (int i = 0; i < n; ++i) {
        fgets(names[i], sizeof(names[i]), stdin);
        names[i][strcspn(names[i], "\n")] = '\0';
    }

    printf("\nOriginal table:\n");
    for (int i = 0; i < n; ++i) {
        printf("%s\n", names[i]);
    }

    for (int i = 0; i < n - 1; ++i) {
        int min_index = i;
        for (int j = i + 1; j < n; ++j) {
            if (strcmp(names[j], names[min_index]) < 0) {
                min_index = j;
            }
        }
        if (min_index != i) {
            swap_names(names, i, min_index);
        }
    }

    printf("\nRearranged table stored back in ascending order:\n");
    for (int i = 0; i < n; ++i) {
        printf("%s\n", names[i]);
    }

    return 0;
}
