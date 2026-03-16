
/*
 * Program 15: Beginner-friendly Linux file manager demo.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}

static void build_path(char *out, size_t size, const char *directory, const char *name) {
    snprintf(out, size, "%s/%s", directory, name);
}

static void list_files(const char *directory) {
    DIR *dir = opendir(directory);
    struct dirent *entry;

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    printf("Files inside %s:\n", directory);
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf(" - %s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(void) {
    const char *workspace = "file_manager_workspace";
    char input[256];

    setbuf(stdout, NULL);

    if (mkdir(workspace, 0777) == -1 && errno != EEXIST) {
        perror("mkdir");
        return EXIT_FAILURE;
    }

    while (1) {
        printf("\nLinux File Manager\n");
        printf("1. List files\n");
        printf("2. Create file\n");
        printf("3. View file\n");
        printf("4. Rename file\n");
        printf("5. Delete file\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        read_line(input, sizeof(input));

        switch (atoi(input)) {
            case 1: {
                list_files(workspace);
                break;
            }
            case 2: {
                char name[128];
                char content[256];
                char path[384];
                FILE *file;

                printf("Enter file name: ");
                read_line(name, sizeof(name));
                printf("Enter one line of content: ");
                read_line(content, sizeof(content));

                build_path(path, sizeof(path), workspace, name);
                file = fopen(path, "w");
                if (file == NULL) {
                    perror("fopen");
                    break;
                }
                fprintf(file, "%s\n", content);
                fclose(file);
                printf("Created %s\n", path);
                break;
            }
            case 3: {
                char name[128];
                char path[384];
                char line[256];
                FILE *file;

                printf("Enter file name to view: ");
                read_line(name, sizeof(name));
                build_path(path, sizeof(path), workspace, name);

                file = fopen(path, "r");
                if (file == NULL) {
                    perror("fopen");
                    break;
                }

                printf("Contents of %s:\n", path);
                while (fgets(line, sizeof(line), file) != NULL) {
                    fputs(line, stdout);
                }
                fclose(file);
                break;
            }
            case 4: {
                char old_name[128];
                char new_name[128];
                char old_path[384];
                char new_path[384];

                printf("Enter current file name: ");
                read_line(old_name, sizeof(old_name));
                printf("Enter new file name: ");
                read_line(new_name, sizeof(new_name));

                build_path(old_path, sizeof(old_path), workspace, old_name);
                build_path(new_path, sizeof(new_path), workspace, new_name);

                if (rename(old_path, new_path) == -1) {
                    perror("rename");
                } else {
                    printf("Renamed %s to %s\n", old_name, new_name);
                }
                break;
            }
            case 5: {
                char name[128];
                char path[384];

                printf("Enter file name to delete: ");
                read_line(name, sizeof(name));
                build_path(path, sizeof(path), workspace, name);

                if (remove(path) == -1) {
                    perror("remove");
                } else {
                    printf("Deleted %s\n", path);
                }
                break;
            }
            case 6:
                return 0;
            default:
                printf("Invalid choice, try again.\n");
                break;
        }
    }
}
