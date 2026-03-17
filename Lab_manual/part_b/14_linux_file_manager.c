#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void read_line(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

int copy_file(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) return -1;
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { close(in); return -1; }
    char buf[8192];
    while(1) {
        ssize_t r = read(in, buf, sizeof(buf));
        if (r == 0) break;
        if (r < 0) { if (errno == EINTR) continue; close(in); close(out); return -1; }
        ssize_t off = 0;
        while (off < r) {
            ssize_t w = write(out, buf + off, (size_t)(r - off));
            if (w < 0) { if (errno == EINTR) continue; close(in); close(out); return -1; }
            off += w;
        }
    }
    close(in);
    close(out);
    return 0;
}

void search_in_dir(const char *dir, const char *name) {
    DIR *d = opendir(dir);
    if (!d) { perror("opendir"); return; }
    struct dirent *e;
    while ((e = readdir(d))) {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        if (strstr(e->d_name, name)) printf("Found: %s/%s\n", dir, e->d_name);
    }
    closedir(d);
}

int main() {
    while(1) {
        puts("\nLinux File Manager (create/delete/copy/move/search)");
        puts("1. Create empty file");
        puts("2. Delete file");
        puts("3. Copy file");
        puts("4. Move (rename) file");
        puts("5. Search in directory (name contains)");
        puts("0. Exit");
        char choice[16];
        read_line("Enter choice: ", choice, sizeof(choice));
        int ch = atoi(choice);
        if (ch == 0) break;

        if (ch == 1) {
            char path[256];
            read_line("File path to create: ", path, sizeof(path));
            int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644);
            if (fd < 0) perror("open");
            else { close(fd); puts("Created."); }
        } else if (ch == 2) {
            char path[256];
            read_line("File path to delete: ", path, sizeof(path));
            if (unlink(path) != 0) perror("unlink");
            else puts("Deleted.");
        } else if (ch == 3) {
            char src[256], dst[256];
            read_line("Source: ", src, sizeof(src));
            read_line("Dest  : ", dst, sizeof(dst));
            if (copy_file(src, dst) != 0) perror("copy");
            else puts("Copied.");
        } else if (ch == 4) {
            char src[256], dst[256];
            read_line("From: ", src, sizeof(src));
            read_line("To  : ", dst, sizeof(dst));
            if (rename(src, dst) != 0) perror("rename");
            else puts("Moved.");
        } else if (ch == 5) {
            char dir[256], name[128];
            read_line("Directory: ", dir, sizeof(dir));
            read_line("Name contains: ", name, sizeof(name));
            search_in_dir(dir, name);
        } else {
            puts("Invalid choice.");
        }
    }
    return 0;
}
