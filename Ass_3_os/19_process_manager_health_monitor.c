
/*
 * Program 19: Manager process checking worker health and restarting failed workers.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct HeartbeatMessage {
    long mtype;
    int worker_id;
    pid_t pid;
};

static volatile sig_atomic_t ping_received = 0;

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

static void handle_ping(int signo) {
    (void)signo;
    ping_received = 1;
}

static pid_t spawn_worker(int queue_id, int worker_id) {
    pid_t pid = fork();

    if (pid == -1) {
        fail("fork");
    }

    if (pid == 0) {
        struct sigaction action;
        int heartbeat_count = 0;

        memset(&action, 0, sizeof(action));
        action.sa_handler = handle_ping;
        if (sigaction(SIGUSR1, &action, NULL) == -1) {
            fail("sigaction");
        }

        while (1) {
            if (!ping_received) {
                pause();
            }
            ping_received = 0;
            heartbeat_count++;

            if (worker_id == 3 && heartbeat_count == 2) {
                printf("Worker %d: simulating malfunction\n", worker_id);
                _exit(EXIT_FAILURE);
            }

            {
                struct HeartbeatMessage message;

                message.mtype = 1;
                message.worker_id = worker_id;
                message.pid = getpid();

                if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                    fail("msgsnd");
                }

                printf("Worker %d: heartbeat sent from pid %d\n", worker_id, (int)getpid());
            }
        }
    }

    return pid;
}

int main(void) {
    int queue_id;
    pid_t workers[5];
    int round;

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    for (int i = 0; i < 5; ++i) {
        workers[i] = spawn_worker(queue_id, i + 1);
    }

    for (round = 1; round <= 5; ++round) {
        int responded[5] = {0, 0, 0, 0, 0};

        printf("Manager process: health-check round %d\n", round);

        /* Ask each worker for a heartbeat in this round. */
        for (int i = 0; i < 5; ++i) {
            kill(workers[i], SIGUSR1);
        }

        sleep_ms(250);

        while (1) {
            struct HeartbeatMessage message;
            ssize_t status = msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 1, IPC_NOWAIT);

            if (status == -1) {
                break;
            }

            responded[message.worker_id - 1] = 1;
        }

        /* Restart any worker that did not answer or already exited. */
        for (int i = 0; i < 5; ++i) {
            int child_status;
            pid_t result = waitpid(workers[i], &child_status, WNOHANG);

            if (result == workers[i] || !responded[i]) {
                printf("Manager process: worker %d malfunctioned, restarting\n", i + 1);
                if (result == 0) {
                    kill(workers[i], SIGTERM);
                    waitpid(workers[i], NULL, 0);
                }
                workers[i] = spawn_worker(queue_id, i + 1);
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        kill(workers[i], SIGTERM);
        waitpid(workers[i], NULL, 0);
    }

    msgctl(queue_id, IPC_RMID, NULL);
    return 0;
}
