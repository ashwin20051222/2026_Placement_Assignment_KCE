#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int server_listen(unsigned short port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) { close(fd); return -1; }
    if (listen(fd, 1) != 0) { close(fd); return -1; }
    return fd;
}

void device_controller(int read_fd) {
    FILE *in = fdopen(read_fd, "r");
    if (!in) exit(1);
    char line[128];
    while (fgets(line, sizeof(line), in)) {
        if (strncmp(line, "LIGHT ON", 8) == 0) puts("[device] light -> ON");
        else if (strncmp(line, "LIGHT OFF", 9) == 0) puts("[device] light -> OFF");
        else if (strncmp(line, "FAN ON", 6) == 0) puts("[device] fan -> ON");
        else if (strncmp(line, "FAN OFF", 7) == 0) puts("[device] fan -> OFF");
        else if (strncmp(line, "QUIT", 4) == 0) break;
        else printf("[device] unknown cmd: %s", line);
    }
    fclose(in);
}

int main() {
    puts("Networking home appliance control (loopback demo)");
    puts("This runs a local TCP server and a client in one program.");

    int p[2];
    if (pipe(p) != 0) { perror("pipe"); return 1; }
    pid_t dev = fork();
    if (dev == 0) {
        close(p[1]);
        device_controller(p[0]);
        exit(0);
    }
    close(p[0]);

    int sfd = server_listen(9099);
    if (sfd < 0) { perror("server"); return 1; }

    pid_t client = fork();
    if (client == 0) {
        sleep(1);
        int cfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(9099);
        if (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) exit(1);
        const char *cmds[] = {"LIGHT ON\n", "FAN ON\n", "FAN OFF\n", "LIGHT OFF\n", "QUIT\n"};
        for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) write(cfd, cmds[i], strlen(cmds[i]));
        close(cfd);
        exit(0);
    }

    int cfd = accept(sfd, NULL, NULL);
    if (cfd < 0) { perror("accept"); return 1; }
    FILE *in = fdopen(cfd, "r");
    if (!in) return 1;
    char line[128];
    while (fgets(line, sizeof(line), in)) {
        write(p[1], line, strlen(line));
        if (strncmp(line, "QUIT", 4) == 0) break;
    }
    fclose(in);
    close(p[1]);
    close(sfd);
    waitpid(client, NULL, 0);
    waitpid(dev, NULL, 0);
    return 0;
}
