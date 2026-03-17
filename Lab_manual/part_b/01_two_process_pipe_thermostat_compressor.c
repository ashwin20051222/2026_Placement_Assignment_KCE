#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

double random_temp() {
    return 18.0 + (double)(rand() % 1701) / 100.0;
}

int main() {
    int pipe_fd[2];
    double temperature;
    int i;

    if (pipe(pipe_fd) != 0) {
        perror("pipe failed");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {

        close(pipe_fd[1]);

        double setpoint   = 26.0;
        double hysteresis = 0.5;
        int compressor_on = 0;

        printf("[child ] Compressor controller started.\n");

        while (1) {

            ssize_t r = read(pipe_fd[0], &temperature, sizeof(temperature));
            if (r <= 0) break;

            if (!compressor_on && temperature >= setpoint + hysteresis)
                compressor_on = 1;
            else if (compressor_on && temperature <= setpoint - hysteresis)
                compressor_on = 0;

            printf("[child ] Temp=%.2f C -> Compressor: %s\n",
                   temperature, compressor_on ? "ON" : "OFF");
        }
        close(pipe_fd[0]);
        exit(0);
    }

    close(pipe_fd[0]);
    srand((unsigned)time(NULL));

    printf("[parent] Thermostat sensor started. Sending 10 readings...\n");

    for (i = 1; i <= 10; i++) {
        temperature = random_temp();
        printf("[parent] Sending temp=%.2f C (reading %d)\n", temperature, i);
        write(pipe_fd[1], &temperature, sizeof(temperature));
        usleep(200000);
    }

    close(pipe_fd[1]);
    waitpid(pid, NULL, 0);
    printf("[parent] Done.\n");
    return 0;
}
