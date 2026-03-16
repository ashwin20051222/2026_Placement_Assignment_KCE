#include <stdio.h>
#include <string.h>

#define MAX_PRODUCTS 100

typedef struct {
    char name[100];
    float price;
} Product;

static void merge(Product products[], int left, int mid, int right) {
    Product temp[MAX_PRODUCTS];
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (products[i].price <= products[j].price) {
            temp[k++] = products[i++];
        } else {
            temp[k++] = products[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = products[i++];
    }

    while (j <= right) {
        temp[k++] = products[j++];
    }

    for (i = left; i <= right; ++i) {
        products[i] = temp[i];
    }
}

static void merge_sort(Product products[], int left, int right) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;
    merge_sort(products, left, mid);
    merge_sort(products, mid + 1, right);
    merge(products, left, mid, right);
}

int main(void) {
    Product products[MAX_PRODUCTS];
    int n;

    printf("Enter number of products: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_PRODUCTS) {
        printf("Invalid number of products.\n");
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        printf("Enter product %d name: ", i + 1);
        fgets(products[i].name, sizeof(products[i].name), stdin);
        products[i].name[strcspn(products[i].name, "\n")] = '\0';
        printf("Enter product %d price: ", i + 1);
        scanf("%f", &products[i].price);
        getchar();
    }

    merge_sort(products, 0, n - 1);

    printf("\nProducts sorted by price using merge sort:\n");
    for (int i = 0; i < n; ++i) {
        printf("%-20s %.2f\n", products[i].name, products[i].price);
    }

    return 0;
}
