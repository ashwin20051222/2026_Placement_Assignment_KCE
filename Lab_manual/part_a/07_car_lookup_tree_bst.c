#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char car_name[128];
    char manufacturer[128];
    char type[64];
    struct Node *left;
    struct Node *right;
};

struct Node *node_new(char *name, char *mfg, char *type) {
    struct Node *n = (struct Node *)malloc(sizeof(struct Node));
    if (n == NULL) return NULL;
    strcpy(n->car_name,    name);
    strcpy(n->manufacturer, mfg);
    strcpy(n->type,        type);
    n->left  = NULL;
    n->right = NULL;
    return n;
}

struct Node *insert(struct Node *root, struct Node *n) {
    if (root == NULL) return n;
    int cmp = strcmp(n->car_name, root->car_name);
    if (cmp < 0)
        root->left  = insert(root->left,  n);
    else if (cmp > 0)
        root->right = insert(root->right, n);
    else {

        strcpy(root->manufacturer, n->manufacturer);
        strcpy(root->type,         n->type);
        free(n);
    }
    return root;
}

struct Node *search(struct Node *root, char *name) {
    while (root != NULL) {
        int cmp = strcmp(name, root->car_name);
        if (cmp == 0) return root;
        else if (cmp < 0) root = root->left;
        else              root = root->right;
    }
    return NULL;
}

void inorder(struct Node *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("  %-20s | %-20s | %s\n",
           root->car_name, root->manufacturer, root->type);
    inorder(root->right);
}

void free_tree(struct Node *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main() {
    struct Node *root = NULL;
    int choice;

    printf("=== Car Lookup System (Binary Search Tree) ===\n");

    while (1) {
        printf("\n1. Add / update car\n");
        printf("2. Search car by name\n");
        printf("3. Display all cars (sorted)\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char name[128], mfg[128], type[64];
            printf("Car name     : "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
            printf("Manufacturer : "); fgets(mfg,  sizeof(mfg),  stdin); mfg[strcspn(mfg,   "\n")] = 0;
            printf("Type (SUV/Sedan/etc): "); fgets(type, sizeof(type), stdin); type[strcspn(type, "\n")] = 0;
            struct Node *n = node_new(name, mfg, type);
            if (n == NULL) { printf("Memory error.\n"); continue; }
            root = insert(root, n);
            printf("Car saved.\n");

        } else if (choice == 2) {
            char name[128];
            printf("Car name to search: "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
            struct Node *found = search(root, name);
            if (found == NULL) printf("Car not found.\n");
            else printf("Found: %s | %s | %s\n", found->car_name, found->manufacturer, found->type);

        } else if (choice == 3) {
            printf("\n--- All Cars (A to Z) ---\n");
            printf("  %-20s | %-20s | %s\n", "Name", "Manufacturer", "Type");
            printf("  %-20s-+-%-20s-+-%s\n", "--------------------", "--------------------", "----");
            if (root == NULL) printf("  (no cars)\n");
            else inorder(root);

        } else {
            printf("Invalid choice.\n");
        }
    }

    free_tree(root);
    return 0;
}
