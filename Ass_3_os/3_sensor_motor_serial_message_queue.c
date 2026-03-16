
/*
 * Program 3: Message queue demo for angle sensor, proximity sensor, and motor control.
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

struct SensorMessage {
    long mtype;
    char sensor_name[16];
    int value;
    int sample_id;
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
    pid_t angle_pid;
    pid_t proximity_pid;
    FILE *serial_file;
    int latest_angle = 90;
    int latest_proximity = 0;
    int received = 0;

    const int angle_samples[] = {15, 25, 40, 70, 85};
    const int proximity_samples[] = {30, 18, 10, 22, 35};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    angle_pid = fork();
    if (angle_pid == -1) {
        fail("fork");
    }
    if (angle_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(angle_samples) / sizeof(angle_samples[0]); ++index) {
            struct SensorMessage message;

            message.mtype = 1;
            strcpy(message.sensor_name, "angle");
            message.value = angle_samples[index];
            message.sample_id = (int)index;

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Angle sensor process: angle=%d degrees\n", message.value);
            sleep_ms(200);
        }

        _exit(EXIT_SUCCESS);
    }

    proximity_pid = fork();
    if (proximity_pid == -1) {
        fail("fork");
    }
    if (proximity_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(proximity_samples) / sizeof(proximity_samples[0]); ++index) {
            struct SensorMessage message;

            message.mtype = 1;
            strcpy(message.sensor_name, "proximity");
            message.value = proximity_samples[index];
            message.sample_id = (int)index;

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Proximity sensor process: distance=%d cm\n", message.value);
            sleep_ms(280);
        }

        _exit(EXIT_SUCCESS);
    }

    serial_file = fopen("motor_serial_port.log", "w");
    if (serial_file == NULL) {
        fail("fopen");
    }

    while (received < 10) {
        struct SensorMessage message;
        int speed;

        if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        if (strcmp(message.sensor_name, "angle") == 0) {
            latest_angle = message.value;
        } else {
            latest_proximity = message.value;
        }

        speed = 120 - (latest_angle / 2) - (latest_proximity * 2);
        if (speed < 20) {
            speed = 20;
        }
        if (speed > 100) {
            speed = 100;
        }

        fprintf(serial_file,
                "sample=%d angle=%d proximity=%d -> SET_MOTOR_SPEED %d\n",
                message.sample_id,
                latest_angle,
                latest_proximity,
                speed);
        fflush(serial_file);

        printf("Controller process: angle=%d, proximity=%d -> serial speed command=%d\n",
               latest_angle,
               latest_proximity,
               speed);
        received++;
    }

    fclose(serial_file);

    waitpid(angle_pid, NULL, 0);
    waitpid(proximity_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
