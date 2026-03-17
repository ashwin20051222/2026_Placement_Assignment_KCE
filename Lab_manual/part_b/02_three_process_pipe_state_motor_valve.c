#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
    CMD_STOP = 0,
    CMD_START = 1,
    CMD_OPEN = 2,
    CMD_CLOSE = 3
};

void motor_process(int read_fd) {
    int running = 0;
    int cmd;
    printf("[motor ] Motor controller started.\n");
    while (read(read_fd, &cmd, sizeof(cmd)) > 0) {
        if      (cmd == CMD_START) running = 1;
        else if (cmd == CMD_STOP)  running = 0;
        printf("[motor ] Command=%d -> Motor is %s\n", cmd, running ? "RUNNING" : "STOPPED");
    }
}

void valve_process(int read_fd) {
    int open_state = 0;
    int cmd;
    printf("[valve ] Valve controller started.\n");
    while (read(read_fd, &cmd, sizeof(cmd)) > 0) {
        if      (cmd == CMD_OPEN)  open_state = 1;
        else if (cmd == CMD_CLOSE) open_state = 0;
        printf("[valve ] Command=%d -> Valve is %s\n", cmd, open_state ? "OPEN" : "CLOSED");
    }
}

int main() {
    int to_motor[2];
    int to_valve[2];
    int i;

    if (pipe(to_motor) != 0 || pipe(to_valve) != 0) {
        perror("pipe failed");
        return 1;
    }

    pid_t motor_pid = fork();
    if (motor_pid < 0) { perror("fork"); return 1; }
    if (motor_pid == 0) {
        close(to_motor[1]);
        close(to_valve[0]);
        close(to_valve[1]);
        motor_process(to_motor[0]);
        close(to_motor[0]);
        exit(0);
    }

    pid_t valve_pid = fork();
    if (valve_pid < 0) { perror("fork"); return 1; }
    if (valve_pid == 0) {
        close(to_valve[1]);
        close(to_motor[0]);
        close(to_motor[1]);
        valve_process(to_valve[0]);
        close(to_valve[0]);
        exit(0);
    }

    close(to_motor[0]);
    close(to_valve[0]);

    int commands[] = {CMD_START, CMD_OPEN, CMD_CLOSE, CMD_STOP, CMD_START, CMD_OPEN, CMD_STOP};
    int num_cmds   = sizeof(commands) / sizeof(commands[0]);

    printf("[parent] State machine sending %d commands...\n", num_cmds);
    for (i = 0; i < num_cmds; i++) {
        printf("[parent] Sending command=%d\n", commands[i]);
        write(to_motor[1], &commands[i], sizeof(int));
        write(to_valve[1], &commands[i], sizeof(int));
        usleep(150000);
    }

    close(to_motor[1]);
    close(to_valve[1]);

    waitpid(motor_pid, NULL, 0);
    waitpid(valve_pid, NULL, 0);
    printf("[parent] All processes done.\n");
    return 0;
}
