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
    int  pressure_kpa;
};

volatile int got_signal = 0;

void signal_handler(int sig) {
    got_signal = 1;
}

void pressure_sensor(int qid, pid_t display_pid) {
    struct Msg m;
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 12; i++) {
        m.mtype        = 1;
        m.pressure_kpa = 80 + (rand() % 61);
        msgsnd(qid, &m, sizeof(int), 0);
        kill(display_pid, SIGUSR1);
        usleep(200000);
    }
}

void pump_controller(int qid) {
    struct Msg m;
    printf("[pump  ] Pump controller started.\n");
    while (1) {
        if (msgrcv(qid, &m, sizeof(int), 1, 0) < 0) break;
        char *state;
        if      (m.pressure_kpa < 95)  state = "ON   (low pressure)";
        else if (m.pressure_kpa > 120) state = "OFF  (high pressure)";
        else                            state = "HOLD (normal range)";
        printf("[pump  ] Pressure=%d kPa -> %s\n", m.pressure_kpa, state);
    }
}

int main() {
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid < 0) { perror("msgget"); return 1; }

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    pid_t pump_pid = fork();
    if (pump_pid == 0) { pump_controller(qid); exit(0); }

    pid_t sensor_pid = fork();
    if (sensor_pid == 0) { pressure_sensor(qid, getppid()); exit(0); }

    printf("[calc  ] Display/calculator waiting for pressure data...\n");
    int shown = 0;
    while (shown < 12) {
        pause();
        if (!got_signal) continue;
        got_signal = 0;

        struct Msg m;
        if (msgrcv(qid, &m, sizeof(int), 1, 0) >= 0) {
            double psi = m.pressure_kpa * 0.145038;
            printf("[calc  ] Pressure=%d kPa = %.1f PSI\n", m.pressure_kpa, psi);
            shown++;
        }
    }

    waitpid(sensor_pid, NULL, 0);
    kill(pump_pid, SIGTERM);
    waitpid(pump_pid, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
