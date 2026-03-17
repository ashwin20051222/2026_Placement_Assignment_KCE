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
    int  a;
    int  b;
};

void user_input_process(int qid) {
    struct Msg m;
    m.mtype = 1;
    printf("[user  ] Enter pump rate (ml/s): ");
    fflush(stdout);
    scanf("%d", &m.a);
    printf("[user  ] Enter time (seconds)  : ");
    fflush(stdout);
    scanf("%d", &m.b);
    msgsnd(qid, &m, sizeof(int) * 2, 0);
}

void pressure_sensor_process(int qid) {
    struct Msg m;
    int i;
    m.mtype = 2;
    m.b = 0;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 8; i++) {
        m.a = 80 + (rand() % 61);
        msgsnd(qid, &m, sizeof(int) * 2, 0);
        usleep(250000);
    }
}

int main() {
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid < 0) { perror("msgget"); return 1; }

    pid_t user_pid   = fork();
    if (user_pid == 0)   { user_input_process(qid);     exit(0); }

    pid_t sensor_pid = fork();
    if (sensor_pid == 0) { pressure_sensor_process(qid); exit(0); }

    struct Msg m;
    int rate = 0, time_s = 0;
    printf("[calc  ] Waiting for pump parameters...\n");
    if (msgrcv(qid, &m, sizeof(int) * 2, 1, 0) >= 0) {
        rate   = m.a;
        time_s = m.b;
        printf("[calc  ] Rate=%d ml/s, Time=%d s -> Volume=%d ml\n",
               rate, time_s, rate * time_s);
    }

    int i;
    for (i = 0; i < 8; i++) {
        if (msgrcv(qid, &m, sizeof(int) * 2, 2, 0) < 0) break;
        int pressure = m.a;
        char *action = (pressure > 120) ? "SLOW DOWN" : "OK";
        printf("[calc  ] Pressure=%d kPa -> Pump: %s\n", pressure, action);
    }

    waitpid(user_pid,   NULL, 0);
    waitpid(sensor_pid, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
