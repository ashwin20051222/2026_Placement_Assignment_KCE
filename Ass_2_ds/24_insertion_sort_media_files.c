#include <stdio.h>
#include <string.h>

#define MAX_FILES 100
#define MAX_NAME 100

int main(void) {
    int n;
    char files[MAX_FILES][MAX_NAME];
    char key[MAX_NAME];

    printf("Enter number of media files: ");
    scanf("%d", &n);
    getchar();

    if (n <= 0 || n > MAX_FILES) {
        printf("Invalid number of files.\n");
        return 0;
    }

    printf("Enter the file names:\n");
    for (int i = 0; i < n; ++i) {
        fgets(files[i], sizeof(files[i]), stdin);
        files[i][strcspn(files[i], "\n")] = '\0';
    }

    for (int i = 1; i < n; ++i) {
        strcpy(key, files[i]);
        int j = i - 1;
        while (j >= 0 && strcmp(files[j], key) > 0) {
            strcpy(files[j + 1], files[j]);
            --j;
        }
        strcpy(files[j + 1], key);
    }

    printf("\nMedia files sorted using insertion sort:\n");
    for (int i = 0; i < n; ++i) {
        printf("%s\n", files[i]);
    }

    return 0;
}
