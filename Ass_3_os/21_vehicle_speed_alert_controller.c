
/*
 * Program 21: Vehicle speed alert control based on wet road condition.
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

struct SpeedMessage {
    long mtype;
    int wet_level;
    int target_speed;
    char alert[32];
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
    pid_t sensor_pid;
    pid_t engine_pid;
    int handled = 0;
    const int wet_samples[] = {10, 35, 60, 75, 20};

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
        for (size_t index = 0; index < sizeof(wet_samples) / sizeof(wet_samples[0]); ++index) {
            struct SpeedMessage message;

            message.mtype = 1;
            message.wet_level = wet_samples[index];

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Wet-condition sensor process: wet level=%d\n", message.wet_level);
            sleep_ms(200);
        }

        _exit(EXIT_SUCCESS);
    }

    engine_pid = fork();
    if (engine_pid == -1) {
        fail("fork");
    }
    if (engine_pid == 0) {
        while (1) {
            struct SpeedMessage command;

            if (msgrcv(queue_id, &command, sizeof(command) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            if (command.target_speed < 0) {
                break;
            }

            printf("Engine control process: target speed reduced to %d km/h\n", command.target_speed);
        }

        _exit(EXIT_SUCCESS);
    }

    while (handled < 5) {
        struct SpeedMessage input;
        struct SpeedMessage command;

        if (msgrcv(queue_id, &input, sizeof(input) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        command.mtype = 2;
        command.wet_level = input.wet_level;

        if (input.wet_level >= 70) {
            strcpy(command.alert, "HIGH");
            command.target_speed = 35;
        } else if (input.wet_level >= 40) {
            strcpy(command.alert, "MEDIUM");
            command.target_speed = 50;
        } else {
            strcpy(command.alert, "LOW");
            command.target_speed = 65;
        }

        printf("Warning process: alert=%s, visual lamp ON, audible chime -> reduce speed\n",
               command.alert);

        if (msgsnd(queue_id, &command, sizeof(command) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        handled++;
    }

    {
        struct SpeedMessage stop_message;

        stop_message.mtype = 2;
        stop_message.target_speed = -1;
        msgsnd(queue_id, &stop_message, sizeof(stop_message) - sizeof(long), 0);
    }

    waitpid(sensor_pid, NULL, 0);
    waitpid(engine_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
