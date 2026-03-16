#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct {
    char name[256];
    int is_directory;
} Entry;

static int compare_entries(const void *a, const void *b) {
    const Entry *left = (const Entry *) a;
    const Entry *right = (const Entry *) b;
    return strcmp(left->name, right->name);
}

static int binary_search_entries(const Entry entries[], int count, const char *target) {
    int left = 0;
    int right = count - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int compare = strcmp(target, entries[mid].name);

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
    char directory_path[PATH_MAX];
    char target[256];
    DIR *directory;
    struct dirent *record;
    Entry *entries = NULL;
    int count = 0;
    int capacity = 0;

    printf("Enter directory path to search in: ");
    if (fgets(directory_path, sizeof(directory_path), stdin) == NULL) {
        return 0;
    }
    directory_path[strcspn(directory_path, "\n")] = '\0';

    directory = opendir(directory_path);
    if (directory == NULL) {
        printf("Unable to open directory.\n");
        return 0;
    }

    while ((record = readdir(directory)) != NULL) {
        if (strcmp(record->d_name, ".") == 0 || strcmp(record->d_name, "..") == 0) {
            continue;
        }

        if (count == capacity) {
            capacity = capacity == 0 ? 16 : capacity * 2;
            Entry *new_entries = realloc(entries, (size_t) capacity * sizeof(*entries));
            if (new_entries == NULL) {
                free(entries);
                closedir(directory);
                printf("Memory allocation failed.\n");
                return 0;
            }
            entries = new_entries;
        }

        snprintf(entries[count].name, sizeof(entries[count].name), "%s", record->d_name);

        char full_path[PATH_MAX];
        struct stat path_info;
        size_t dir_len = strlen(directory_path);
        size_t name_len = strlen(record->d_name);

        if (dir_len + 1 + name_len < sizeof(full_path)) {
            memcpy(full_path, directory_path, dir_len);
            full_path[dir_len] = '/';
            memcpy(full_path + dir_len + 1, record->d_name, name_len + 1);

            if (stat(full_path, &path_info) == 0 && S_ISDIR(path_info.st_mode)) {
                entries[count].is_directory = 1;
            } else {
                entries[count].is_directory = 0;
            }
        } else {
            entries[count].is_directory = 0;
        }

        ++count;
    }

    closedir(directory);

    if (count == 0) {
        free(entries);
        printf("No files or directories found.\n");
        return 0;
    }

    qsort(entries, (size_t) count, sizeof(*entries), compare_entries);

    printf("Enter file or directory name to search: ");
    fgets(target, sizeof(target), stdin);
    target[strcspn(target, "\n")] = '\0';

    int index = binary_search_entries(entries, count, target);
    if (index >= 0) {
        printf("%s found. Type: %s\n",
               entries[index].name,
               entries[index].is_directory ? "Directory" : "File");
    } else {
        printf("Entry not found.\n");
    }

    free(entries);
    return 0;
}
