#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

struct Msg {
    long mtype;
    int  value;
};

void sensor_process(int qid, long mtype, int min_val, int max_val, char *sensor_name) {
    struct Msg m;
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 10; i++) {
        m.mtype = mtype;
        m.value = min_val + (rand() % (max_val - min_val + 1));
        msgsnd(qid, &m, sizeof(int), 0);
        printf("[%s] Sent value: %d\n", sensor_name, m.value);
        usleep(150000);
    }
}

int main() {

    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid < 0) { perror("msgget"); return 1; }

    pid_t angle_pid = fork();
    if (angle_pid == 0) {
        sensor_process(qid, 1, 0, 180, "angle");
        exit(0);
    }

    pid_t prox_pid = fork();
    if (prox_pid == 0) {
        sensor_process(qid, 2, 0, 100, "prox");
        exit(0);
    }

    int last_angle = 0;
    int last_prox  = 100;
    int i;
    struct Msg m;

    printf("[motor ] Motor controller waiting for sensor data...\n");

    for (i = 0; i < 20; i++) {

        if (msgrcv(qid, &m, sizeof(int), 0, 0) < 0) break;

        if (m.mtype == 1) last_angle = m.value;
        if (m.mtype == 2) last_prox  = m.value;

        char *decision;
        if (last_prox < 20)
            decision = "STOP (obstacle ahead!)";
        else if (last_angle > 90)
            decision = "TURN RIGHT";
        else
            decision = "TURN LEFT";

        printf("[motor ] Angle=%3d deg, Prox=%3d cm -> %s\n",
               last_angle, last_prox, decision);
    }

    waitpid(angle_pid, NULL, 0);
    waitpid(prox_pid,  NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
