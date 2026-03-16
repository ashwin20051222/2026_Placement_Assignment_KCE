
/*
 * Program 16: Health parameter file writer and simple prescription server.
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
    const int port = 46016;
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
        char buffer[256];
        float temperature = 0.0f;
        int systolic_bp = 0;
        int spo2 = 0;
        const char *prescription = "Hydrate well and continue regular monitoring.";

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

        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer) - 1);

        sscanf(buffer, "temperature=%f,bp=%d,spo2=%d", &temperature, &systolic_bp, &spo2);

        if (temperature > 38.0f || systolic_bp > 140 || spo2 < 94) {
            prescription = "Consult a doctor, take rest, and repeat vital checks.";
        } else if (temperature > 37.2f || systolic_bp > 130) {
            prescription = "Maintain hydration, reduce exertion, and watch symptoms.";
        }

        write(client_fd, prescription, strlen(prescription));
        close(client_fd);
        close(server_fd);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        FILE *file = fopen("health_parameters.txt", "w");
        int client_fd;
        struct sockaddr_in address;
        char response[256] = {0};
        const char *payload = "temperature=37.8,bp=132,spo2=96";

        if (file == NULL) {
            fail("fopen");
        }

        fprintf(file, "temperature=37.8\nbp=132\nspo2=96\n");
        fclose(file);

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

        write(client_fd, payload, strlen(payload));
        read(client_fd, response, sizeof(response) - 1);

        printf("Health parameters saved in health_parameters.txt\n");
        printf("Server prescription: %s\n", response);

        close(client_fd);
    }

    waitpid(server_pid, NULL, 0);
    return 0;
}
