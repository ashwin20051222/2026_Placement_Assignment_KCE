
/*
 * Program 2: State machine sending pipe commands to valve and motor processes.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct MotorCommand {
    int step_id;
    int direction;
};

struct ValveCommand {
    int step_id;
    int inlet_open;
    int outlet_open;
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
    int motor_pipe[2];
    int valve_pipe[2];
    pid_t motor_pid;
    pid_t valve_pid;
    pid_t state_pid;

    const struct MotorCommand motor_sequence[] = {
        {1, 0},
        {2, 1},
        {3, -1},
        {4, 0},
        {-1, 0}
    };
    const struct ValveCommand valve_sequence[] = {
        {1, 1, 0},
        {2, 0, 0},
        {3, 0, 0},
        {4, 0, 1},
        {-1, 0, 0}
    };

    setbuf(stdout, NULL);

    if (pipe(motor_pipe) == -1 || pipe(valve_pipe) == -1) {
        fail("pipe");
    }

    motor_pid = fork();
    if (motor_pid == -1) {
        fail("fork");
    }
    if (motor_pid == 0) {
        struct MotorCommand command;

        close(motor_pipe[1]);
        close(valve_pipe[0]);
        close(valve_pipe[1]);

        while (read(motor_pipe[0], &command, sizeof(command)) == (ssize_t)sizeof(command)) {
            if (command.step_id == -1) {
                break;
            }

            if (command.direction > 0) {
                printf("Motor process: step %d -> rotate FORWARD\n", command.step_id);
            } else if (command.direction < 0) {
                printf("Motor process: step %d -> rotate BACKWARD\n", command.step_id);
            } else {
                printf("Motor process: step %d -> STOP\n", command.step_id);
            }
        }

        close(motor_pipe[0]);
        _exit(EXIT_SUCCESS);
    }

    valve_pid = fork();
    if (valve_pid == -1) {
        fail("fork");
    }
    if (valve_pid == 0) {
        struct ValveCommand command;

        close(valve_pipe[1]);
        close(motor_pipe[0]);
        close(motor_pipe[1]);

        while (read(valve_pipe[0], &command, sizeof(command)) == (ssize_t)sizeof(command)) {
            if (command.step_id == -1) {
                break;
            }

            printf("Valve process: step %d -> inlet %s, outlet %s\n",
                   command.step_id,
                   command.inlet_open ? "OPEN" : "CLOSED",
                   command.outlet_open ? "OPEN" : "CLOSED");
        }

        close(valve_pipe[0]);
        _exit(EXIT_SUCCESS);
    }

    state_pid = fork();
    if (state_pid == -1) {
        fail("fork");
    }
    if (state_pid == 0) {
        size_t index;

        close(motor_pipe[0]);
        close(valve_pipe[0]);

        for (index = 0; index < sizeof(motor_sequence) / sizeof(motor_sequence[0]); ++index) {
            if (write(motor_pipe[1], &motor_sequence[index], sizeof(motor_sequence[index])) !=
                (ssize_t)sizeof(motor_sequence[index])) {
                fail("write");
            }
            if (write(valve_pipe[1], &valve_sequence[index], sizeof(valve_sequence[index])) !=
                (ssize_t)sizeof(valve_sequence[index])) {
                fail("write");
            }

            if (motor_sequence[index].step_id != -1) {
                printf("State machine: issued command for step %d\n", motor_sequence[index].step_id);
                sleep_ms(300);
            }
        }

        close(motor_pipe[1]);
        close(valve_pipe[1]);
        _exit(EXIT_SUCCESS);
    }

    close(motor_pipe[0]);
    close(motor_pipe[1]);
    close(valve_pipe[0]);
    close(valve_pipe[1]);

    waitpid(state_pid, NULL, 0);
    waitpid(motor_pid, NULL, 0);
    waitpid(valve_pid, NULL, 0);

    return 0;
}
