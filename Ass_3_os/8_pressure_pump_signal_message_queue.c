
/*
 * Program 8: Pressure-on-request example using signals and message queues.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct PressureMessage {
    long mtype;
    float pressure;
    float pump_speed;
};

static volatile sig_atomic_t request_pending = 0;

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

static void handle_request_signal(int signo) {
    (void)signo;
    request_pending = 1;
}

int main(void) {
    int queue_id;
    pid_t sensor_pid;
    pid_t pump_pid;
    int cycle;
    const float pressure_samples[] = {0.8f, 1.1f, 1.5f, 1.2f};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    sensor_pid = fork();
    if (sensor_pid == -1) {
        fail("fork");
    }
    if (sensor_pid == 0) {
        struct sigaction action;
        int handled = 0;

        memset(&action, 0, sizeof(action));
        action.sa_handler = handle_request_signal;

        if (sigaction(SIGUSR1, &action, NULL) == -1) {
            fail("sigaction");
        }

        while (handled < 4) {
            if (!request_pending) {
                pause();
            }

            if (request_pending) {
                struct PressureMessage message;

                request_pending = 0;
                message.mtype = 1;
                message.pressure = pressure_samples[handled];
                message.pump_speed = 0.0f;

                if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                    fail("msgsnd");
                }

                printf("Pressure sensor process: request served, pressure=%.1f bar\n", message.pressure);
                handled++;
            }
        }

        _exit(EXIT_SUCCESS);
    }

    pump_pid = fork();
    if (pump_pid == -1) {
        fail("fork");
    }
    if (pump_pid == 0) {
        for (cycle = 0; cycle < 4; ++cycle) {
            struct PressureMessage message;

            if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            printf("Pump control process: pressure=%.1f bar -> speed=%.1f rpm\n",
                   message.pressure,
                   message.pump_speed);
        }

        _exit(EXIT_SUCCESS);
    }

    for (cycle = 0; cycle < 4; ++cycle) {
        struct PressureMessage sensor_message;
        struct PressureMessage pump_message;

        kill(sensor_pid, SIGUSR1);

        if (msgrcv(queue_id, &sensor_message, sizeof(sensor_message) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        pump_message.mtype = 2;
        pump_message.pressure = sensor_message.pressure;
        pump_message.pump_speed = 150.0f - sensor_message.pressure * 45.0f;

        printf("Calculation/display process: pressure=%.1f -> command pump speed %.1f rpm\n",
               sensor_message.pressure,
               pump_message.pump_speed);

        if (msgsnd(queue_id, &pump_message, sizeof(pump_message) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        sleep_ms(200);
    }

    waitpid(sensor_pid, NULL, 0);
    waitpid(pump_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
