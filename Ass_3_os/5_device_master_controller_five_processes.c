
/*
 * Program 5: Five-process device controller with simple master logic.
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

struct DeviceMessage {
    long mtype;
    int sensor_value;
    int user_start;
    char action[64];
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

static void worker_loop(int queue_id, long type, const char *name) {
    while (1) {
        struct DeviceMessage message;

        if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), type, 0) == -1) {
            fail("msgrcv");
        }

        if (strcmp(message.action, "shutdown") == 0) {
            printf("%s process: stopping\n", name);
            break;
        }

        printf("%s process: %s\n", name, message.action);
    }
}

int main(void) {
    int queue_id;
    pid_t valve_pid;
    pid_t motor_pid;
    pid_t output_pid;
    pid_t reader_pid;
    int handled = 0;

    const int levels[] = {10, 32, 55, 41, 12};
    const int user_start[] = {1, 1, 1, 0, 1};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    valve_pid = fork();
    if (valve_pid == -1) {
        fail("fork");
    }
    if (valve_pid == 0) {
        worker_loop(queue_id, 1, "Valve");
        _exit(EXIT_SUCCESS);
    }

    motor_pid = fork();
    if (motor_pid == -1) {
        fail("fork");
    }
    if (motor_pid == 0) {
        worker_loop(queue_id, 2, "Motor");
        _exit(EXIT_SUCCESS);
    }

    output_pid = fork();
    if (output_pid == -1) {
        fail("fork");
    }
    if (output_pid == 0) {
        worker_loop(queue_id, 3, "Light/Relay/Buzzer");
        _exit(EXIT_SUCCESS);
    }

    reader_pid = fork();
    if (reader_pid == -1) {
        fail("fork");
    }
    if (reader_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(levels) / sizeof(levels[0]); ++index) {
            struct DeviceMessage message;

            message.mtype = 10;
            message.sensor_value = levels[index];
            message.user_start = user_start[index];
            strcpy(message.action, "reader_update");

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Sensor/user input process: level=%d, start=%d\n",
                   message.sensor_value,
                   message.user_start);
            sleep_ms(220);
        }

        _exit(EXIT_SUCCESS);
    }

    while (handled < 5) {
        struct DeviceMessage input_message;
        struct DeviceMessage worker_message;

        if (msgrcv(queue_id, &input_message, sizeof(input_message) - sizeof(long), 10, 0) == -1) {
            fail("msgrcv");
        }

        if (!input_message.user_start) {
            worker_message.mtype = 2;
            strcpy(worker_message.action, "stop both motors");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 1;
            strcpy(worker_message.action, "close all solenoid valves");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 3;
            strcpy(worker_message.action, "turn light off, relay off, buzzer off");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);
        } else if (input_message.sensor_value > 40) {
            worker_message.mtype = 1;
            strcpy(worker_message.action, "open drain valve and close inlet valve");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 2;
            strcpy(worker_message.action, "run motor A forward and motor B low speed");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 3;
            strcpy(worker_message.action, "turn warning light on and buzzer beep");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);
        } else {
            worker_message.mtype = 1;
            strcpy(worker_message.action, "open inlet valve, close drain valve");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 2;
            strcpy(worker_message.action, "run both motors at normal speed");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);

            worker_message.mtype = 3;
            strcpy(worker_message.action, "turn status light on and buzzer off");
            msgsnd(queue_id, &worker_message, sizeof(worker_message) - sizeof(long), 0);
        }

        printf("Main controller process: handled input %d\n", handled + 1);
        handled++;
    }

    {
        long types[] = {1, 2, 3};
        size_t index;

        for (index = 0; index < sizeof(types) / sizeof(types[0]); ++index) {
            struct DeviceMessage message;

            message.mtype = types[index];
            strcpy(message.action, "shutdown");
            msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0);
        }
    }

    waitpid(valve_pid, NULL, 0);
    waitpid(motor_pid, NULL, 0);
    waitpid(output_pid, NULL, 0);
    waitpid(reader_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
