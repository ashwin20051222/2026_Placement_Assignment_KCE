
/*
 * Program 23: Simple network speed test using local TCP sockets.
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

static double elapsed_seconds(struct timespec start, struct timespec end) {
    double seconds = (double)(end.tv_sec - start.tv_sec);
    double nanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    double total = seconds + nanoseconds;

    return total > 0.0 ? total : 0.000001;
}

int main(void) {
    const int port = 46023;
    const size_t total_bytes = 4 * 1024 * 1024;
    const size_t chunk_size = 64 * 1024;
    pid_t server_pid;

    setbuf(stdout, NULL);

    server_pid = fork();
    if (server_pid == -1) {
        fail("fork");
    }

    if (server_pid == 0) {
        /* Child process works like a small local speed-test server. */
        int server_fd;
        int client_fd;
        int reuse = 1;
        struct sockaddr_in address;
        char buffer[64 * 1024];
        size_t transferred = 0;

        memset(buffer, 'S', sizeof(buffer));

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

        while (transferred < total_bytes) {
            size_t remaining = total_bytes - transferred;
            size_t current = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
            ssize_t written = write(client_fd, buffer, current);

            if (written <= 0) {
                fail("write");
            }
            transferred += (size_t)written;
        }

        transferred = 0;
        while (transferred < total_bytes) {
            ssize_t received = read(client_fd, buffer, sizeof(buffer));

            if (received <= 0) {
                break;
            }
            transferred += (size_t)received;
        }

        close(client_fd);
        close(server_fd);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        /* Parent process connects as the client and measures transfer time. */
        int client_fd;
        struct sockaddr_in address;
        char *buffer = malloc(chunk_size);
        struct timespec start;
        struct timespec end;
        size_t transferred = 0;
        double seconds;
        double mbps;

        if (buffer == NULL) {
            fail("malloc");
        }
        memset(buffer, 'C', chunk_size);

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

        clock_gettime(CLOCK_MONOTONIC, &start);
        while (transferred < total_bytes) {
            ssize_t received = read(client_fd, buffer, chunk_size);

            if (received <= 0) {
                fail("read");
            }
            transferred += (size_t)received;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        seconds = elapsed_seconds(start, end);
        mbps = ((double)transferred * 8.0) / (seconds * 1000000.0);
        printf("Download speed: %.2f Mbps (%zu bytes in %.4f s)\n", mbps, transferred, seconds);

        transferred = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        while (transferred < total_bytes) {
            size_t remaining = total_bytes - transferred;
            size_t current = remaining < chunk_size ? remaining : chunk_size;
            ssize_t written = write(client_fd, buffer, current);

            if (written <= 0) {
                fail("write");
            }
            transferred += (size_t)written;
        }
        shutdown(client_fd, SHUT_WR);
        clock_gettime(CLOCK_MONOTONIC, &end);

        seconds = elapsed_seconds(start, end);
        mbps = ((double)transferred * 8.0) / (seconds * 1000000.0);
        printf("Upload speed: %.2f Mbps (%zu bytes in %.4f s)\n", mbps, transferred, seconds);

        free(buffer);
        close(client_fd);
    }

    waitpid(server_pid, NULL, 0);
    return 0;
}
