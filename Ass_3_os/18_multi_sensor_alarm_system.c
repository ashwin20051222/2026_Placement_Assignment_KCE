
/*
 * Program 18: Multi-sensor alarm system with visual and audible priority output.
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

struct AlarmMessage {
    long mtype;
    int sensor_id;
    int value;
    int priority;
    char color[16];
    char tone[16];
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

static void sensor_process(int queue_id, int sensor_id, const int *samples, size_t count) {
    for (size_t index = 0; index < count; ++index) {
        struct AlarmMessage message;

        message.mtype = 1;
        message.sensor_id = sensor_id;
        message.value = samples[index];
        message.priority = 0;
        strcpy(message.color, "none");
        strcpy(message.tone, "silent");

        if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        printf("Sensor %d process: value=%d\n", sensor_id, message.value);
        sleep_ms(180 + sensor_id * 40);
    }
}

int main(void) {
    int queue_id;
    pid_t sensor1_pid;
    pid_t sensor2_pid;
    pid_t sensor3_pid;
    pid_t alarm_output_pid;
    int received = 0;

    const int sensor1_samples[] = {15, 45, 72, 30};
    const int sensor2_samples[] = {25, 55, 88, 60};
    const int sensor3_samples[] = {10, 35, 67, 95};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    sensor1_pid = fork();
    if (sensor1_pid == -1) {
        fail("fork");
    }
    if (sensor1_pid == 0) {
        sensor_process(queue_id, 1, sensor1_samples, sizeof(sensor1_samples) / sizeof(sensor1_samples[0]));
        _exit(EXIT_SUCCESS);
    }

    sensor2_pid = fork();
    if (sensor2_pid == -1) {
        fail("fork");
    }
    if (sensor2_pid == 0) {
        sensor_process(queue_id, 2, sensor2_samples, sizeof(sensor2_samples) / sizeof(sensor2_samples[0]));
        _exit(EXIT_SUCCESS);
    }

    sensor3_pid = fork();
    if (sensor3_pid == -1) {
        fail("fork");
    }
    if (sensor3_pid == 0) {
        sensor_process(queue_id, 3, sensor3_samples, sizeof(sensor3_samples) / sizeof(sensor3_samples[0]));
        _exit(EXIT_SUCCESS);
    }

    alarm_output_pid = fork();
    if (alarm_output_pid == -1) {
        fail("fork");
    }
    if (alarm_output_pid == 0) {
        while (1) {
            struct AlarmMessage alarm;

            if (msgrcv(queue_id, &alarm, sizeof(alarm) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            if (alarm.sensor_id == -1) {
                break;
            }

            printf("Alarm output process: sensor=%d priority=%d visual=%s audible=%s\n",
                   alarm.sensor_id,
                   alarm.priority,
                   alarm.color,
                   alarm.tone);
        }

        _exit(EXIT_SUCCESS);
    }

    while (received < 12) {
        struct AlarmMessage input;
        struct AlarmMessage alarm;

        if (msgrcv(queue_id, &input, sizeof(input) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        alarm.mtype = 2;
        alarm.sensor_id = input.sensor_id;
        alarm.value = input.value;

        if (input.value >= 85) {
            alarm.priority = 3;
            strcpy(alarm.color, "RED");
            strcpy(alarm.tone, "FAST_BEEP");
        } else if (input.value >= 60) {
            alarm.priority = 2;
            strcpy(alarm.color, "YELLOW");
            strcpy(alarm.tone, "MEDIUM_BEEP");
        } else if (input.value >= 35) {
            alarm.priority = 1;
            strcpy(alarm.color, "BLUE");
            strcpy(alarm.tone, "SLOW_BEEP");
        } else {
            alarm.priority = 0;
            strcpy(alarm.color, "GREEN");
            strcpy(alarm.tone, "NONE");
        }

        printf("Limit check process: sensor=%d value=%d -> priority=%d\n",
               input.sensor_id,
               input.value,
               alarm.priority);

        if (msgsnd(queue_id, &alarm, sizeof(alarm) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }
        received++;
    }

    {
        struct AlarmMessage stop_message;

        stop_message.mtype = 2;
        stop_message.sensor_id = -1;
        msgsnd(queue_id, &stop_message, sizeof(stop_message) - sizeof(long), 0);
    }

    waitpid(sensor1_pid, NULL, 0);
    waitpid(sensor2_pid, NULL, 0);
    waitpid(sensor3_pid, NULL, 0);
    waitpid(alarm_output_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
