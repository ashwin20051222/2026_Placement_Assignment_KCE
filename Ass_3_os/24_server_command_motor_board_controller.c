
/*
 * Program 24: Server command receiver for AC, DC, and stepper motor control.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct MotorCommand {
    char command[64];
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
    const int port = 46024;
    int control_pipe[2];
    pid_t server_pid;
    pid_t controller_pid;

    setbuf(stdout, NULL);

    if (pipe(control_pipe) == -1) {
        fail("pipe");
    }

    controller_pid = fork();
    if (controller_pid == -1) {
        fail("fork");
    }
    if (controller_pid == 0) {
        struct MotorCommand command;

        close(control_pipe[1]);
        while (read(control_pipe[0], &command, sizeof(command)) == (ssize_t)sizeof(command)) {
            if (strcmp(command.command, "STOP") == 0) {
                break;
            }

            if (strncmp(command.command, "AC", 2) == 0) {
                printf("Motor board process: AC motor command -> %s\n", command.command);
            } else if (strncmp(command.command, "DC", 2) == 0) {
                printf("Motor board process: DC motor command -> %s\n", command.command);
            } else {
                printf("Motor board process: Stepper motor command -> %s\n", command.command);
            }
        }

        close(control_pipe[0]);
        _exit(EXIT_SUCCESS);
    }

    server_pid = fork();
    if (server_pid == -1) {
        fail("fork");
    }
    if (server_pid == 0) {
        int server_fd;
        int client_fd;
        int reuse = 1;
        struct sockaddr_in address;
        const char *commands[] = {"AC:ON", "DC:FORWARD:1200", "STEPPER:ROTATE:90", "STOP"};

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            fail("socket");
        }

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons((unsigned short)port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            fail("bind");
        }
        if (listen(server_fd, 1) == -1) {
            fail("listen");
        }

        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            fail("accept");
        }

        for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
            dprintf(client_fd, "%s\n", commands[i]);
            sleep_ms(150);
        }

        close(client_fd);
        close(server_fd);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        int receiver_fd;
        struct sockaddr_in address;
        FILE *stream;
        char line[64];

        close(control_pipe[0]);

        receiver_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (receiver_fd == -1) {
            fail("socket");
        }

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons((unsigned short)port);

        if (connect(receiver_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            fail("connect");
        }

        stream = fdopen(receiver_fd, "r");
        if (stream == NULL) {
            fail("fdopen");
        }

        while (fgets(line, sizeof(line), stream) != NULL) {
            struct MotorCommand command;

            line[strcspn(line, "\n")] = '\0';
            strncpy(command.command, line, sizeof(command.command) - 1);
            command.command[sizeof(command.command) - 1] = '\0';

            printf("Receiver process: command from server -> %s\n", command.command);

            if (write(control_pipe[1], &command, sizeof(command)) != (ssize_t)sizeof(command)) {
                fail("write");
            }

            if (strcmp(command.command, "STOP") == 0) {
                break;
            }
        }

        fclose(stream);
        close(control_pipe[1]);
    }

    waitpid(server_pid, NULL, 0);
    waitpid(controller_pid, NULL, 0);
    return 0;
}
