
/*
 * Program 17: Server and client sharing memory with semaphore synchronization.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SharedChannel {
    sem_t client_ready;
    sem_t server_ready;
    char request[128];
    char response[128];
};

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(void) {
    struct SharedChannel *channel;
    pid_t client_pid;

    setbuf(stdout, NULL);

    channel = mmap(NULL, sizeof(*channel), PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (channel == MAP_FAILED) {
        fail("mmap");
    }

    if (sem_init(&channel->client_ready, 1, 0) == -1 ||
        sem_init(&channel->server_ready, 1, 0) == -1) {
        fail("sem_init");
    }

    memset(channel->request, 0, sizeof(channel->request));
    memset(channel->response, 0, sizeof(channel->response));

    client_pid = fork();
    if (client_pid == -1) {
        fail("fork");
    }

    if (client_pid == 0) {
        strcpy(channel->request, "READ shared diagnostics");
        printf("Client process: wrote request into shared memory\n");
        sem_post(&channel->client_ready);
        sem_wait(&channel->server_ready);
        printf("Client process: response from server -> %s\n", channel->response);
        _exit(EXIT_SUCCESS);
    }

    sem_wait(&channel->client_ready);
    printf("Server process: request received -> %s\n", channel->request);
    snprintf(channel->response,
             sizeof(channel->response),
             "Diagnostics OK: %.100s",
             channel->request);
    sem_post(&channel->server_ready);

    waitpid(client_pid, NULL, 0);
    sem_destroy(&channel->client_ready);
    sem_destroy(&channel->server_ready);
    munmap(channel, sizeof(*channel));

    return 0;
}
