
/*
 * Program 4: Message queue based microwave-style device controller.
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
    int value;
    char text[32];
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
    pid_t light_pid;
    pid_t fan_pid;
    pid_t thermostat_pid;
    int set_temperature = 68;
    int door_closed = 1;
    int handled = 0;

    const int thermostat_samples[] = {62, 65, 69, 71, 67, 64};
    const int door_samples[] = {1, 1, 0, 0, 1, 1};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    light_pid = fork();
    if (light_pid == -1) {
        fail("fork");
    }
    if (light_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(door_samples) / sizeof(door_samples[0]); ++index) {
            printf("Light and door process: door %s -> light %s\n",
                   door_samples[index] ? "CLOSED" : "OPEN",
                   door_samples[index] ? "OFF" : "ON");
            sleep_ms(250);
        }

        _exit(EXIT_SUCCESS);
    }

    fan_pid = fork();
    if (fan_pid == -1) {
        fail("fork");
    }
    if (fan_pid == 0) {
        while (1) {
            struct DeviceMessage message;

            if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            if (message.value < 0) {
                break;
            }

            printf("Fan and turntable process: heater %s -> fan %s, turntable %s\n",
                   message.value ? "ON" : "OFF",
                   message.value ? "ON" : "OFF",
                   message.value ? "ROTATING" : "STOPPED");
        }

        _exit(EXIT_SUCCESS);
    }

    thermostat_pid = fork();
    if (thermostat_pid == -1) {
        fail("fork");
    }
    if (thermostat_pid == 0) {
        size_t index;

        for (index = 0; index < sizeof(thermostat_samples) / sizeof(thermostat_samples[0]); ++index) {
            struct DeviceMessage message;

            message.mtype = 1;
            message.value = thermostat_samples[index];
            strcpy(message.text, "thermostat");

            if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                fail("msgsnd");
            }

            printf("Thermostat process: sensed %d C\n", message.value);
            sleep_ms(220);
        }

        _exit(EXIT_SUCCESS);
    }

    while (handled < 6) {
        struct DeviceMessage incoming;
        struct DeviceMessage outgoing;
        int heater_on;

        if (msgrcv(queue_id, &incoming, sizeof(incoming) - sizeof(long), 1, 0) == -1) {
            fail("msgrcv");
        }

        door_closed = door_samples[handled];
        heater_on = (incoming.value < set_temperature && door_closed) ? 1 : 0;

        printf("Heater process: temp=%d C, set=%d C, door=%s -> heater %s\n",
               incoming.value,
               set_temperature,
               door_closed ? "CLOSED" : "OPEN",
               heater_on ? "ON" : "OFF");

        outgoing.mtype = 2;
        outgoing.value = heater_on;
        strcpy(outgoing.text, "heater_state");

        if (msgsnd(queue_id, &outgoing, sizeof(outgoing) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        handled++;
    }

    {
        struct DeviceMessage stop_message;

        stop_message.mtype = 2;
        stop_message.value = -1;
        strcpy(stop_message.text, "stop");
        msgsnd(queue_id, &stop_message, sizeof(stop_message) - sizeof(long), 0);
    }

    waitpid(light_pid, NULL, 0);
    waitpid(fan_pid, NULL, 0);
    waitpid(thermostat_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
