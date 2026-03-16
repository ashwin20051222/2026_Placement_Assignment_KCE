
/*
 * Program 1: Pipe communication between thermostat reader and compressor controller.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct TemperatureReading {
    double current_temperature;
    double set_temperature;
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
    int pipefd[2];
    pid_t reader_pid;
    pid_t controller_pid;

    setbuf(stdout, NULL);

    if (pipe(pipefd) == -1) {
        fail("pipe");
    }

    reader_pid = fork();
    if (reader_pid == -1) {
        fail("fork");
    }

    if (reader_pid == 0) {
        int raw_sensor_values[] = {225, 238, 251, 261, 248, 236};
        double set_temperature = 24.5;
        size_t count = sizeof(raw_sensor_values) / sizeof(raw_sensor_values[0]);
        size_t index;

        close(pipefd[0]);

        for (index = 0; index < count; ++index) {
            struct TemperatureReading reading;

            reading.current_temperature = raw_sensor_values[index] / 10.0;
            reading.set_temperature = set_temperature;

            if (write(pipefd[1], &reading, sizeof(reading)) != (ssize_t)sizeof(reading)) {
                fail("write");
            }

            printf("Read process: raw=%d, temperature=%.1f C, target=%.1f C\n",
                   raw_sensor_values[index],
                   reading.current_temperature,
                   reading.set_temperature);
            sleep_ms(250);
        }

        close(pipefd[1]);
        _exit(EXIT_SUCCESS);
    }

    controller_pid = fork();
    if (controller_pid == -1) {
        fail("fork");
    }

    if (controller_pid == 0) {
        struct TemperatureReading reading;

        close(pipefd[1]);

        while (read(pipefd[0], &reading, sizeof(reading)) == (ssize_t)sizeof(reading)) {
            const char *compressor_state =
                (reading.current_temperature > reading.set_temperature + 0.5) ? "ON" : "OFF";

            printf("Control process: current=%.1f C -> compressor %s\n",
                   reading.current_temperature,
                   compressor_state);
        }

        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(reader_pid, NULL, 0);
    waitpid(controller_pid, NULL, 0);

    return 0;
}
