
/*
 * Program 22: Traffic signal control using nearby signal information.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct TrafficMessage {
    long mtype;
    int signal_id;
    int round;
    int vehicle_count;
    int green_time;
};

static void sleep_ms(long milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

static void signal_process(int queue_id, int signal_id, const int *counts, size_t count) {
    for (size_t round = 0; round < count; ++round) {
        struct TrafficMessage report;
        struct TrafficMessage decision;

        report.mtype = 1;
        report.signal_id = signal_id;
        report.round = (int)round + 1;
        report.vehicle_count = counts[round];
        report.green_time = 0;

        if (msgsnd(queue_id, &report, sizeof(report) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        if (msgrcv(queue_id, &decision, sizeof(decision) - sizeof(long), 100 + signal_id, 0) == -1) {
            fail("msgrcv");
        }

        printf("Signal %d process: nearby traffic=%d -> green time %d seconds\n",
               signal_id,
               report.vehicle_count,
               decision.green_time);
        sleep_ms(180);
    }
}

int main(void) {
    int queue_id;
    pid_t signals[4];

    const int north_counts[] = {18, 35, 20};
    const int east_counts[] = {30, 25, 42};
    const int south_counts[] = {14, 40, 24};
    const int west_counts[] = {22, 18, 28};
    const int *traffic_data[] = {north_counts, east_counts, south_counts, west_counts};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    for (int i = 0; i < 4; ++i) {
        signals[i] = fork();
        if (signals[i] == -1) {
            fail("fork");
        }
        if (signals[i] == 0) {
            signal_process(queue_id, i + 1, traffic_data[i], 3);
            _exit(EXIT_SUCCESS);
        }
    }

    for (int round = 1; round <= 3; ++round) {
        struct TrafficMessage reports[4];
        int busiest_count = 0;

        for (int i = 0; i < 4; ++i) {
            if (msgrcv(queue_id, &reports[i], sizeof(reports[i]) - sizeof(long), 1, 0) == -1) {
                fail("msgrcv");
            }
            if (reports[i].vehicle_count > busiest_count) {
                busiest_count = reports[i].vehicle_count;
            }
        }

        printf("Controller: round %d received nearby signal counts, busiest lane=%d vehicles\n",
               round,
               busiest_count);

        for (int i = 0; i < 4; ++i) {
            struct TrafficMessage decision;

            decision.mtype = 100 + reports[i].signal_id;
            decision.signal_id = reports[i].signal_id;
            decision.round = round;
            decision.vehicle_count = reports[i].vehicle_count;
            if (busiest_count == 0) {
                decision.green_time = 20;
            } else {
                decision.green_time = 20 + (reports[i].vehicle_count * 30 / busiest_count);
            }

            if (msgsnd(queue_id, &decision, sizeof(decision) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        waitpid(signals[i], NULL, 0);
    }

    msgctl(queue_id, IPC_RMID, NULL);
    return 0;
}
