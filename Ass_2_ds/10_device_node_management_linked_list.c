#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Device {
    int id;
    char name[100];
    char type[100];
    struct Device *next;
} Device;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Device *create_device(int id, const char *name, const char *type) {
    Device *device = (Device *) malloc(sizeof(Device));
    if (device == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    device->id = id;
    strncpy(device->name, name, sizeof(device->name) - 1);
    device->name[sizeof(device->name) - 1] = '\0';
    strncpy(device->type, type, sizeof(device->type) - 1);
    device->type[sizeof(device->type) - 1] = '\0';
    device->next = NULL;
    return device;
}

static void add_device(Device **head, int id, const char *name, const char *type) {
    Device *device = create_device(id, name, type);
    if (*head == NULL) {
        *head = device;
        return;
    }

    Device *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = device;
}

static void delete_device(Device **head, int id) {
    Device *temp = *head;
    Device *prev = NULL;

    while (temp != NULL && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Device not found.\n");
        return;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
    printf("Device deleted successfully.\n");
}

static void display_devices(Device *head) {
    if (head == NULL) {
        printf("No devices available.\n");
        return;
    }

    printf("\nDevice list:\n");
    while (head != NULL) {
        printf("ID: %-5d Name: %-15s Type: %s\n", head->id, head->name, head->type);
        head = head->next;
    }
}

static void free_devices(Device *head) {
    while (head != NULL) {
        Device *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    Device *head = NULL;
    int choice;
    int id;
    char name[100];
    char type[100];

    while (1) {
        printf("\nDevice Node Menu\n");
        printf("1. Add device node\n");
        printf("2. Delete device node\n");
        printf("3. Display devices\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter device ID: ");
                scanf("%d", &id);
                getchar();
                printf("Enter device name: ");
                read_line(name, sizeof(name));
                printf("Enter device type: ");
                read_line(type, sizeof(type));
                add_device(&head, id, name, type);
                break;
            case 2:
                printf("Enter device ID to delete: ");
                scanf("%d", &id);
                getchar();
                delete_device(&head, id);
                break;
            case 3:
                display_devices(head);
                break;
            case 4:
                free_devices(head);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_devices(head);
    return 0;
}
