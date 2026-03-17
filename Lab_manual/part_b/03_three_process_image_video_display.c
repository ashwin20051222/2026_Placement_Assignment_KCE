#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

struct Frame {
    char kind;
    int  seq;
    char name[64];
};

void writer_process(char kind, int write_fd) {
    struct Frame f;
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    for (i = 1; i <= 5; i++) {
        f.kind = kind;
        f.seq  = i;
        if (kind == 'J')
            snprintf(f.name, sizeof(f.name), "photo_%d.jpg", i);
        else
            snprintf(f.name, sizeof(f.name), "video_frame_%d.3gp", i);

        write(write_fd, &f, sizeof(f));
        printf("[writer-%c] Sent frame %d: %s\n", kind, i, f.name);
        usleep(100000 + rand() % 200000);
    }
}

int main() {
    int pipe_fd[2];

    if (pipe(pipe_fd) != 0) {
        perror("pipe failed");
        return 1;
    }

    pid_t jpg_pid = fork();
    if (jpg_pid < 0) { perror("fork"); return 1; }
    if (jpg_pid == 0) {
        close(pipe_fd[0]);
        writer_process('J', pipe_fd[1]);
        close(pipe_fd[1]);
        exit(0);
    }

    pid_t vid_pid = fork();
    if (vid_pid < 0) { perror("fork"); return 1; }
    if (vid_pid == 0) {
        close(pipe_fd[0]);
        writer_process('V', pipe_fd[1]);
        close(pipe_fd[1]);
        exit(0);
    }

    close(pipe_fd[1]);

    printf("[display] Waiting for frames...\n");
    struct Frame f;
    while (read(pipe_fd[0], &f, sizeof(f)) == sizeof(f)) {
        printf("[display] Received %s frame %d: %s\n",
               (f.kind == 'J') ? "JPEG" : "Video", f.seq, f.name);
    }

    close(pipe_fd[0]);
    waitpid(jpg_pid, NULL, 0);
    waitpid(vid_pid, NULL, 0);
    printf("[display] All frames received.\n");
    return 0;
}
