
/*
 * Program 28: Vehicle trip client-server file storage example.
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
    const int port = 46028;
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
        FILE *network_stream;
        FILE *trip_file;
        char line[256];

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

        network_stream = fdopen(client_fd, "r");
        if (network_stream == NULL) {
            fail("fdopen");
        }

        trip_file = fopen("vehicle_trip_information.txt", "w");
        if (trip_file == NULL) {
            fail("fopen");
        }

        while (fgets(line, sizeof(line), network_stream) != NULL) {
            fputs(line, trip_file);
            printf("Server: stored trip record -> %s", line);
        }

        fclose(trip_file);
        fclose(network_stream);
        close(server_fd);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        int client_fd;
        struct sockaddr_in address;
        FILE *stream;
        const char *trips[] = {
            "trip_id=9001,start=08:15,end=08:55,distance_km=22.4,avg_speed=41\n",
            "trip_id=9002,start=10:05,end=10:40,distance_km=18.2,avg_speed=37\n",
            "trip_id=9003,start=13:10,end=13:58,distance_km=28.9,avg_speed=44\n"
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

        for (size_t i = 0; i < sizeof(trips) / sizeof(trips[0]); ++i) {
            fputs(trips[i], stream);
            fflush(stream);
            sleep_ms(120);
        }

        fclose(stream);
    }

    waitpid(server_pid, NULL, 0);
    return 0;
}
