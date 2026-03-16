
/*
 * Program 20: Circular buffer with shared memory and semaphores.
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

struct SharedBuffer {
    sem_t empty_slots;
    sem_t full_slots;
    sem_t mutex;
    unsigned short buffer[8];
    int head;
    int tail;
};

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(void) {
    struct SharedBuffer *shared;
    pid_t producer_pid;
    unsigned short samples[6] = {0x1201, 0x1202, 0x1203, 0x1204, 0x1205, 0x1206};

    setbuf(stdout, NULL);

    shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        fail("mmap");
    }

    if (sem_init(&shared->empty_slots, 1, 8) == -1 ||
        sem_init(&shared->full_slots, 1, 0) == -1 ||
        sem_init(&shared->mutex, 1, 1) == -1) {
        fail("sem_init");
    }

    shared->head = 0;
    shared->tail = 0;

    producer_pid = fork();
    if (producer_pid == -1) {
        fail("fork");
    }

    if (producer_pid == 0) {
        for (int i = 0; i < 6; ++i) {
            sem_wait(&shared->empty_slots);
            sem_wait(&shared->mutex);

            shared->buffer[shared->tail] = samples[i];
            printf("Writer process: wrote sensor value 0x%04X at tail=%d\n", samples[i], shared->tail);
            shared->tail = (shared->tail + 1) % 8;

            sem_post(&shared->mutex);
            sem_post(&shared->full_slots);
        }

        _exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < 6; ++i) {
        unsigned short value;

        sem_wait(&shared->full_slots);
        sem_wait(&shared->mutex);

        value = shared->buffer[shared->head];
        printf("Reader process: read sensor value 0x%04X from head=%d, next tail=%d\n",
               value,
               shared->head,
               shared->tail);
        shared->head = (shared->head + 1) % 8;

        sem_post(&shared->mutex);
        sem_post(&shared->empty_slots);
    }

    waitpid(producer_pid, NULL, 0);
    sem_destroy(&shared->empty_slots);
    sem_destroy(&shared->full_slots);
    sem_destroy(&shared->mutex);
    munmap(shared, sizeof(*shared));

    return 0;
}
