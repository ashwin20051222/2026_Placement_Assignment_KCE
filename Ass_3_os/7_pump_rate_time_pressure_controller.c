
/*
 * Program 7: Pump control using user values, pressure input, and message queues.
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

struct PumpMessage {
    long mtype;
    float rate;
    float duration;
    float pressure;
    float speed;
    char direction[16];
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

int main(void) {
    int queue_id;
    pid_t input_pid;
    pid_t pump_pid;
    pid_t pressure_pid;
    float rate = 12.0f;
    float duration = 6.0f;
    int received_status = 0;

    const float pressure_samples[] = {1.1f, 1.4f, 1.6f, 1.2f};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    input_pid = fork();
    if (input_pid == -1) {
        fail("fork");
    }
    if (input_pid == 0) {
        struct PumpMessage message;

        printf("Enter pump rate (ml/h) and time (h). Default is 12 and 6: ");
        if (scanf("%f %f", &rate, &duration) != 2) {
            rate = 12.0f;
            duration = 6.0f;
        }

        message.mtype = 1;
        message.rate = rate;
        message.duration = duration;
        message.pressure = 0.0f;
        message.speed = 0.0f;
        strcpy(message.direction, "forward");

        if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        printf("Input process: rate=%.1f ml/h, time=%.1f h\n", rate, duration);
        _exit(EXIT_SUCCESS);
    }

    pump_pid = fork();
    if (pump_pid == -1) {
        fail("fork");
    }
    if (pump_pid == 0) {
        struct PumpMessage command;
        int handled = 0;

        if (msgrcv(queue_id, &command, sizeof(command) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        while (handled < 4) {
            struct PumpMessage pressure_message;
            struct PumpMessage status_message;

            if (msgrcv(queue_id, &pressure_message, sizeof(pressure_message) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            status_message.mtype = 3;
            status_message.rate = command.rate;
            status_message.duration = command.duration;
            status_message.pressure = pressure_message.pressure;
            status_message.speed = command.rate * 4.0f - pressure_message.pressure * 8.0f;
            strcpy(status_message.direction, status_message.pressure > 1.5f ? "reverse" : "forward");

            printf("Pump control process: pressure=%.1f bar -> direction=%s, speed=%.1f rpm\n",
                   status_message.pressure,
                   status_message.direction,
                   status_message.speed);

            if (msgsnd(queue_id, &status_message, sizeof(status_message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            handled++;
        }

        _exit(EXIT_SUCCESS);
    }

    pressure_pid = fork();
    if (pressure_pid == -1) {
        fail("fork");
    }
    if (pressure_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(pressure_samples) / sizeof(pressure_samples[0]); ++index) {
            struct PumpMessage message;

            message.mtype = 2;
            message.pressure = pressure_samples[index];

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Pressure sensor process: pressure=%.1f bar\n", message.pressure);
            sleep_ms(250);
        }

        _exit(EXIT_SUCCESS);
    }

    while (received_status < 4) {
        struct PumpMessage status;

        if (msgrcv(queue_id, &status, sizeof(status) - sizeof(long), 3, 0) == -1) {
            fail("msgrcv");
        }

        printf("Monitor process: flow rate=%.1f, time=%.1f, direction=%s, speed=%.1f rpm\n",
               status.rate,
               status.duration,
               status.direction,
               status.speed);
        received_status++;
    }

    waitpid(input_pid, NULL, 0);
    waitpid(pump_pid, NULL, 0);
    waitpid(pressure_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
