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
    int  value;
};

void send_msg(int qid, long mtype, int value) {
    struct Msg m;
    m.mtype = mtype;
    m.value = value;
    msgsnd(qid, &m, sizeof(int), 0);
}

void thermostat_reader(int qid) {
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 12; i++) {
        int temp = 18 + (rand() % 16);
        send_msg(qid, 1, temp);
        printf("[thermo] Temperature = %d C\n", temp);
        usleep(200000);
    }
}

void light_door_monitor(int qid) {
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 10; i++) {
        int door_open = rand() % 2;
        int light_on  = door_open;
        printf("[monitor] Door=%s  Light=%s\n",
               door_open ? "OPEN" : "CLOSED",
               light_on  ? "ON"   : "OFF");
        send_msg(qid, 2, door_open);
        usleep(250000);
    }
}

void heater_controller(int qid) {
    struct Msg m;
    printf("[heater ] Heater controller waiting...\n");
    while (1) {
        if (msgrcv(qid, &m, sizeof(int), 3, 0) < 0) break;
        printf("[heater ] -> %s\n", m.value ? "ON" : "OFF");
    }
}

void fan_controller(int qid) {
    struct Msg m;
    printf("[fan    ] Fan controller waiting...\n");
    while (1) {
        if (msgrcv(qid, &m, sizeof(int), 4, 0) < 0) break;
        printf("[fan    ] -> %s\n", m.value ? "ON" : "OFF");
    }
}

int main() {
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid < 0) { perror("msgget"); return 1; }

    pid_t heater_pid = fork();
    if (heater_pid == 0) { heater_controller(qid); exit(0); }

    pid_t fan_pid = fork();
    if (fan_pid == 0)    { fan_controller(qid);    exit(0); }

    pid_t thermo_pid = fork();
    if (thermo_pid == 0) { thermostat_reader(qid); exit(0); }

    pid_t mon_pid = fork();
    if (mon_pid == 0)    { light_door_monitor(qid); exit(0); }

    int setpoint = 24;
    struct Msg m;
    int i;
    for (i = 0; i < 12; i++) {
        if (msgrcv(qid, &m, sizeof(int), 1, 0) < 0) break;
        int temp     = m.value;
        int heat_on  = (temp < setpoint)     ? 1 : 0;
        int fan_on   = (temp > setpoint + 2) ? 1 : 0;
        printf("[super ] Temp=%d -> Heater=%s Fan=%s\n",
               temp, heat_on ? "ON" : "OFF", fan_on ? "ON" : "OFF");
        send_msg(qid, 3, heat_on);
        send_msg(qid, 4, fan_on);
    }

    waitpid(thermo_pid, NULL, 0);
    waitpid(mon_pid,    NULL, 0);
    kill(heater_pid, SIGTERM);
    kill(fan_pid,    SIGTERM);
    waitpid(heater_pid, NULL, 0);
    waitpid(fan_pid,    NULL, 0);

    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
