#include <stdio.h>
#include <string.h>

#define MAX_FLIGHTS 100

typedef struct {
    char flight_number[20];
    int arrival_time;
} Flight;

static void bubble_sort(Flight flights[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (flights[j].arrival_time > flights[j + 1].arrival_time) {
                Flight temp = flights[j];
                flights[j] = flights[j + 1];
                flights[j + 1] = temp;
            }
        }
    }
}

static void display_flights(const Flight flights[], int n) {
    if (n == 0) {
        printf("Arrival table is empty.\n");
        return;
    }

    printf("\nArrival table:\n");
    for (int i = 0; i < n; ++i) {
        printf("Flight: %-10s Arrival time: %04d\n", flights[i].flight_number, flights[i].arrival_time);
    }
}

int main(void) {
    Flight flights[MAX_FLIGHTS];
    int count = 0;
    int choice;

    while (1) {
        printf("\nFlight Arrival Menu\n");
        printf("1. Add new flight arrival\n");
        printf("2. Display sorted arrival table\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                if (count >= MAX_FLIGHTS) {
                    printf("Arrival table is full.\n");
                    break;
                }
                printf("Enter flight number: ");
                scanf("%19s", flights[count].flight_number);
                printf("Enter arrival time in HHMM format: ");
                scanf("%d", &flights[count].arrival_time);
                ++count;
                bubble_sort(flights, count);
                printf("Flight added and table sorted using bubble sort.\n");
                break;
            case 2:
                display_flights(flights, count);
                break;
            case 3:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
