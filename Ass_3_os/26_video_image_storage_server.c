
/*
 * Program 26: Server example that stores received video/image data in a file.
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
    const int port = 46026;
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
        char filename[128];
        char size_text[32];
        long size;
        FILE *output;
        char buffer[256];

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

        if (fgets(filename, sizeof(filename), stream) == NULL ||
            fgets(size_text, sizeof(size_text), stream) == NULL) {
            fprintf(stderr, "Invalid client header\n");
            fclose(stream);
            close(server_fd);
            _exit(EXIT_FAILURE);
        }

        filename[strcspn(filename, "\n")] = '\0';
        size = strtol(size_text, NULL, 10);

        output = fopen(filename, "wb");
        if (output == NULL) {
            fail("fopen");
        }

        while (size > 0) {
            size_t to_read = (size > (long)sizeof(buffer)) ? sizeof(buffer) : (size_t)size;
            size_t received = fread(buffer, 1, to_read, stream);

            if (received == 0) {
                break;
            }

            fwrite(buffer, 1, received, output);
            size -= (long)received;
        }

        fclose(output);
        fclose(stream);
        close(server_fd);

        printf("Server: stored incoming video image in %s\n", filename);
        _exit(EXIT_SUCCESS);
    }

    sleep_ms(200);

    {
        int client_fd;
        struct sockaddr_in address;
        FILE *stream;
        const char payload[] = "SIMULATED_VIDEO_IMAGE_PAYLOAD_BLOCK";
        const char *filename = "received_video_image.bin";

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

        fprintf(stream, "%s\n%zu\n", filename, sizeof(payload) - 1);
        fwrite(payload, 1, sizeof(payload) - 1, stream);
        fflush(stream);
        fclose(stream);
    }

    waitpid(server_pid, NULL, 0);
    return 0;
}
