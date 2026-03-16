#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CarNode {
    char car_name[100];
    char manufacturer[100];
    char type[100];
    struct CarNode *left;
    struct CarNode *right;
} CarNode;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static CarNode *create_car(const char *name, const char *manufacturer, const char *type) {
    CarNode *node = (CarNode *) malloc(sizeof(CarNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(node->car_name, name, sizeof(node->car_name) - 1);
    node->car_name[sizeof(node->car_name) - 1] = '\0';
    strncpy(node->manufacturer, manufacturer, sizeof(node->manufacturer) - 1);
    node->manufacturer[sizeof(node->manufacturer) - 1] = '\0';
    strncpy(node->type, type, sizeof(node->type) - 1);
    node->type[sizeof(node->type) - 1] = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

static CarNode *insert_car(CarNode *root, const char *name, const char *manufacturer, const char *type) {
    if (root == NULL) {
        return create_car(name, manufacturer, type);
    }

    int compare = strcmp(name, root->car_name);
    if (compare < 0) {
        root->left = insert_car(root->left, name, manufacturer, type);
    } else if (compare > 0) {
        root->right = insert_car(root->right, name, manufacturer, type);
    } else {
        strncpy(root->manufacturer, manufacturer, sizeof(root->manufacturer) - 1);
        root->manufacturer[sizeof(root->manufacturer) - 1] = '\0';
        strncpy(root->type, type, sizeof(root->type) - 1);
        root->type[sizeof(root->type) - 1] = '\0';
    }
    return root;
}

static CarNode *search_car(CarNode *root, const char *name) {
    while (root != NULL) {
        int compare = strcmp(name, root->car_name);
        if (compare == 0) {
            return root;
        }
        root = (compare < 0) ? root->left : root->right;
    }
    return NULL;
}

static void inorder(CarNode *root) {
    if (root == NULL) {
        return;
    }
    inorder(root->left);
    printf("Car: %-20s Manufacturer: %-15s Type: %s\n", root->car_name, root->manufacturer, root->type);
    inorder(root->right);
}

static void free_tree(CarNode *root) {
    if (root == NULL) {
        return;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void) {
    CarNode *root = NULL;
    int choice;
    char name[100];
    char manufacturer[100];
    char type[100];

    while (1) {
        printf("\nCar Lookup Tree Menu\n");
        printf("1. Add or update car\n");
        printf("2. Search by car name\n");
        printf("3. Display all cars\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter car name: ");
                read_line(name, sizeof(name));
                printf("Enter manufacturer: ");
                read_line(manufacturer, sizeof(manufacturer));
                printf("Enter car type: ");
                read_line(type, sizeof(type));
                root = insert_car(root, name, manufacturer, type);
                break;
            case 2:
                printf("Enter car name to search: ");
                read_line(name, sizeof(name));
                {
                    CarNode *result = search_car(root, name);
                    if (result != NULL) {
                        printf("Manufacturer: %s\nType: %s\n", result->manufacturer, result->type);
                    } else {
                        printf("Car not found.\n");
                    }
                }
                break;
            case 3:
                if (root == NULL) {
                    printf("Tree is empty.\n");
                } else {
                    inorder(root);
                }
                break;
            case 4:
                free_tree(root);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_tree(root);
    return 0;
}
