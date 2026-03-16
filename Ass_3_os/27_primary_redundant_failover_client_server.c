
/*
 * Program 27: Primary and redundant system communication with failover idea.
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
    const int port = 46027;
    pid_t server_pid;

    setbuf(stdout, NULL);

    server_pid = fork();
    if (server_pid == -1) {
        fail("fork");
    }

    if (server_pid == 0) {
        int server_fd;
        int client_fd;
        int reuse = 1;
        struct sockaddr_in address;
        FILE *stream;
        char line[128];

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

        stream = fdopen(client_fd, "r");
        if (stream == NULL) {
            fail("fdopen");
        }

        while (fgets(line, sizeof(line), stream) != NULL) {
            line[strcspn(line, "\n")] = '\0';
            printf("Redundant system server: primary state -> %s\n", line);
        }

        printf("Redundant system server: primary failure detected, taking over services now\n");

        fclose(stream);
        close(server_fd);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        int client_fd;
        struct sockaddr_in address;
        FILE *stream;
        const char *states[] = {
            "RUNNING: pumps=ON, valves=OPEN",
            "RUNNING: pumps=ON, valves=CLOSED",
            "RUNNING: health=GOOD",
            "RUNNING: throughput=stable"
        };

        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd == -1) {
            fail("socket");
        }

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons((unsigned short)port);

        if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            fail("connect");
        }

        stream = fdopen(client_fd, "w");
        if (stream == NULL) {
            fail("fdopen");
        }

        for (size_t i = 0; i < sizeof(states) / sizeof(states[0]); ++i) {
            fprintf(stream, "%s\n", states[i]);
            fflush(stream);
            printf("Primary client: state sent -> %s\n", states[i]);
            sleep_ms(180);
        }

        printf("Primary client: simulating primary system failure\n");
        fclose(stream);
    }

    waitpid(server_pid, NULL, 0);
    return 0;
}
