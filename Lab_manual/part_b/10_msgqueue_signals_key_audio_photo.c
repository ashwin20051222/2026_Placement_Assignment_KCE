#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

struct Msg {
    long mtype;
    int  event_id;
};

void audio_process(int qid) {
    struct Msg m;
    int count = 0;
    printf("[audio ] Audio capture process ready.\n");
    while (1) {
        if (msgrcv(qid, &m, sizeof(int), 1, 0) < 0) break;
        count++;
        printf("[audio ] Recording audio clip #%d (event_id=%d)\n", count, m.event_id);
    }
}

void photo_process(int qid) {
    struct Msg m;
    int count = 0;
    printf("[photo ] Photo capture process ready.\n");
    while (1) {
        if (msgrcv(qid, &m, sizeof(int), 2, 0) < 0) break;
        count++;
        printf("[photo ] Taking photo #%d (event_id=%d)\n", count, m.event_id);
    }
}

void key_listener(int qid) {
    struct Msg m;
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 1; i <= 10; i++) {

        m.event_id = i;
        if (i % 2 == 0) {
            m.mtype = 1;
            printf("[keys  ] Key press -> AUDIO event %d\n", i);
        } else {
            m.mtype = 2;
            printf("[keys  ] Key press -> PHOTO event %d\n", i);
        }
        msgsnd(qid, &m, sizeof(int), 0);
        usleep(200000);
    }
}

int main() {
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid < 0) { perror("msgget"); return 1; }

    pid_t audio_pid = fork();
    if (audio_pid == 0) { audio_process(qid); exit(0); }

    pid_t photo_pid = fork();
    if (photo_pid == 0) { photo_process(qid); exit(0); }

    pid_t key_pid = fork();
    if (key_pid == 0)   { key_listener(qid);  exit(0); }

    waitpid(key_pid, NULL, 0);
    kill(audio_pid, SIGTERM);
    kill(photo_pid, SIGTERM);
    waitpid(audio_pid, NULL, 0);
    waitpid(photo_pid, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    printf("[main  ] All done.\n");
    return 0;
}
