
/*
 * Program 6: Shared memory example for time, temperature, humidity, and display.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct SharedEnvironment {
    char current_time[32];
    float temperature;
    float humidity;
    int selection;
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

static void update_clock(struct SharedEnvironment *shared) {
    int cycle;

    for (cycle = 0; cycle < 5; ++cycle) {
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);

        if (tm_now == NULL) {
            fail("localtime");
        }

        strftime(shared->current_time, sizeof(shared->current_time), "%H:%M:%S", tm_now);
        sleep_ms(180);
    }
}

int main(void) {
    int shared_id;
    struct SharedEnvironment *shared;
    pid_t clock_pid;
    pid_t temperature_pid;
    pid_t humidity_pid;
    int selection = 2;
    int index;

    const float temperatures[] = {24.2f, 24.6f, 25.0f, 25.1f, 24.7f};
    const float humidities[] = {50.0f, 51.5f, 52.4f, 51.9f, 50.8f};

    setbuf(stdout, NULL);

    printf("Display mode: 1=video, 2=dashboard. Default is 2: ");
    if (scanf("%d", &selection) != 1) {
        selection = 2;
    }

    shared_id = shmget(IPC_PRIVATE, sizeof(*shared), IPC_CREAT | 0600);
    if (shared_id == -1) {
        fail("shmget");
    }

    shared = shmat(shared_id, NULL, 0);
    if (shared == (void *)-1) {
        fail("shmat");
    }

    memset(shared, 0, sizeof(*shared));
    shared->selection = selection;

    clock_pid = fork();
    if (clock_pid == -1) {
        fail("fork");
    }
    if (clock_pid == 0) {
        update_clock(shared);
        _exit(EXIT_SUCCESS);
    }

    temperature_pid = fork();
    if (temperature_pid == -1) {
        fail("fork");
    }
    if (temperature_pid == 0) {
        int cycle;

        for (cycle = 0; cycle < 5; ++cycle) {
            shared->temperature = temperatures[cycle];
            sleep_ms(210);
        }

        _exit(EXIT_SUCCESS);
    }

    humidity_pid = fork();
    if (humidity_pid == -1) {
        fail("fork");
    }
    if (humidity_pid == 0) {
        int cycle;

        for (cycle = 0; cycle < 5; ++cycle) {
            shared->humidity = humidities[cycle];
            sleep_ms(230);
        }

        _exit(EXIT_SUCCESS);
    }

    for (index = 0; index < 5; ++index) {
        sleep_ms(260);

        if (shared->selection == 1) {
            printf("Display process: playing promo video while status is cached in memory\n");
        } else {
            printf("Display process: time=%s, temperature=%.1f C, humidity=%.1f %%\n",
                   shared->current_time,
                   shared->temperature,
                   shared->humidity);
        }
    }

    waitpid(clock_pid, NULL, 0);
    waitpid(temperature_pid, NULL, 0);
    waitpid(humidity_pid, NULL, 0);

    shmdt(shared);
    shmctl(shared_id, IPC_RMID, NULL);

    return 0;
}
