#include <stdio.h>
#include <string.h>

#define MAX_CHEMICALS 100

typedef struct {
    char name[100];
    int hazard_level;
} Chemical;

static void swap(Chemical *a, Chemical *b) {
    Chemical temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(Chemical chemicals[], int low, int high) {
    int pivot = chemicals[high].hazard_level;
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (chemicals[j].hazard_level >= pivot) {
            ++i;
            swap(&chemicals[i], &chemicals[j]);
        }
    }

    swap(&chemicals[i + 1], &chemicals[high]);
    return i + 1;
}

static void quick_sort(Chemical chemicals[], int low, int high) {
    if (low < high) {
        int pivot_index = partition(chemicals, low, high);
        quick_sort(chemicals, low, pivot_index - 1);
        quick_sort(chemicals, pivot_index + 1, high);
    }
}

int main(void) {
    Chemical chemicals[MAX_CHEMICALS];
    int n;

    printf("Enter number of hazardous materials: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_CHEMICALS) {
        printf("Invalid number of materials.\n");
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        printf("Enter chemical %d name: ", i + 1);
        fgets(chemicals[i].name, sizeof(chemicals[i].name), stdin);
        chemicals[i].name[strcspn(chemicals[i].name, "\n")] = '\0';
        printf("Enter hazard level for %s: ", chemicals[i].name);
        scanf("%d", &chemicals[i].hazard_level);
        getchar();
    }

    quick_sort(chemicals, 0, n - 1);

    printf("\nChemicals sorted by hazard level using quick sort:\n");
    for (int i = 0; i < n; ++i) {
        printf("%-20s Hazard Level: %d\n", chemicals[i].name, chemicals[i].hazard_level);
    }

    return 0;
}
