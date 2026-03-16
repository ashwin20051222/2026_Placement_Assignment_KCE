
/*
 * Program 9: Mixed IPC example using message queue, signal, semaphore, and shared memory.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct CommandMessage {
    long mtype;
    int sequence_id;
    int target_speed;
    char command[32];
};

struct SharedState {
    sem_t mutex;
    int latest_sequence_id;
    int target_speed;
    int periodic_calculation;
};

static volatile sig_atomic_t calculation_ready = 0;

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

static void handle_calculation_signal(int signo) {
    (void)signo;
    calculation_ready = 1;
}

int main(void) {
    int queue_id;
    struct SharedState *shared;
    pid_t motor_pid;
    pid_t calculator_pid;
    int sequence_id;

    const char *commands[] = {"align motor", "start conveyor", "rotate arm", "stop sequence"};
    const int target_speeds[] = {30, 45, 25, 0};

    setbuf(stdout, NULL);

    shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        fail("mmap");
    }

    if (sem_init(&shared->mutex, 1, 1) == -1) {
        fail("sem_init");
    }

    /* Start the shared values from zero so each process reads clean data. */
    shared->latest_sequence_id = 0;
    shared->target_speed = 0;
    shared->periodic_calculation = 0;

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    motor_pid = fork();
    if (motor_pid == -1) {
        fail("fork");
    }
    if (motor_pid == 0) {
        struct sigaction action;
        int handled = 0;

        /* The motor process waits for a signal when new calculated data is ready. */
        memset(&action, 0, sizeof(action));
        action.sa_handler = handle_calculation_signal;
        if (sigaction(SIGUSR1, &action, NULL) == -1) {
            fail("sigaction");
        }

        while (handled < 4) {
            struct CommandMessage message;

            if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 1, 0) == -1) {
                fail("msgrcv");
            }

            printf("Motor process: sequence %d -> %s at target speed %d\n",
                   message.sequence_id,
                   message.command,
                   message.target_speed);

            if (calculation_ready) {
                sem_wait(&shared->mutex);
                printf("Motor process: received periodic calculation %d from shared memory\n",
                       shared->periodic_calculation);
                sem_post(&shared->mutex);
                calculation_ready = 0;
            }

            handled++;
        }

        _exit(EXIT_SUCCESS);
    }

    calculator_pid = fork();
    if (calculator_pid == -1) {
        fail("fork");
    }
    if (calculator_pid == 0) {
        for (sequence_id = 0; sequence_id < 4; ++sequence_id) {
            sleep_ms(260);

            /* Read the latest shared values and store the periodic calculation. */
            sem_wait(&shared->mutex);
            shared->periodic_calculation =
                shared->target_speed * 3 + shared->latest_sequence_id * 7;
            sem_post(&shared->mutex);

            kill(motor_pid, SIGUSR1);
            printf("Calculation process: updated periodic value in shared memory\n");
        }

        _exit(EXIT_SUCCESS);
    }

    for (sequence_id = 0; sequence_id < 4; ++sequence_id) {
        struct CommandMessage message;

        /* The input part updates the shared memory and also sends the queue command. */
        sem_wait(&shared->mutex);
        shared->latest_sequence_id = sequence_id + 1;
        shared->target_speed = target_speeds[sequence_id];
        sem_post(&shared->mutex);

        message.mtype = 1;
        message.sequence_id = sequence_id + 1;
        message.target_speed = target_speeds[sequence_id];
        strcpy(message.command, commands[sequence_id]);

        if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
            fail("msgsnd");
        }

        printf("Input process: user selected sequence %d with target speed %d\n",
               message.sequence_id,
               message.target_speed);
        sleep_ms(200);
    }

    waitpid(motor_pid, NULL, 0);
    waitpid(calculator_pid, NULL, 0);

    sem_destroy(&shared->mutex);
    munmap(shared, sizeof(*shared));
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
