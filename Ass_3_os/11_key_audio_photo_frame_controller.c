
/*
 * Program 11: Key input, audio player, and photo frame coordination example.
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

struct KeyMessage {
    long mtype;
    char key[16];
};

static volatile sig_atomic_t audio_interrupt = 0;
static volatile sig_atomic_t photo_interrupt = 0;

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

static void handle_audio_signal(int signo) {
    (void)signo;
    audio_interrupt = 1;
}

static void handle_photo_signal(int signo) {
    (void)signo;
    photo_interrupt = 1;
}

int main(void) {
    int queue_id;
    pid_t audio_pid;
    pid_t photo_pid;
    pid_t key_pid;
    const char *keys[] = {"^", "-->", "v", "<--", "backspace"};

    setbuf(stdout, NULL);

    queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (queue_id == -1) {
        fail("msgget");
    }

    audio_pid = fork();
    if (audio_pid == -1) {
        fail("fork");
    }
    if (audio_pid == 0) {
        struct sigaction action;
        int handled = 0;

        /* This child handles only audio related keys. */
        memset(&action, 0, sizeof(action));
        action.sa_handler = handle_audio_signal;
        if (sigaction(SIGUSR1, &action, NULL) == -1) {
            fail("sigaction");
        }

        while (handled < 3) {
            struct KeyMessage message;

            if (!audio_interrupt) {
                pause();
            }
            audio_interrupt = 0;

            if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 1, 0) == -1) {
                fail("msgrcv");
            }

            if (strcmp(message.key, "^") == 0) {
                printf("Audio player process: volume UP interrupt handled\n");
            } else if (strcmp(message.key, "v") == 0) {
                printf("Audio player process: volume DOWN interrupt handled\n");
            } else {
                printf("Audio player process: exit command received\n");
            }
            handled++;
        }

        _exit(EXIT_SUCCESS);
    }

    photo_pid = fork();
    if (photo_pid == -1) {
        fail("fork");
    }
    if (photo_pid == 0) {
        struct sigaction action;
        int handled = 0;

        /* This child handles only photo-frame related keys. */
        memset(&action, 0, sizeof(action));
        action.sa_handler = handle_photo_signal;
        if (sigaction(SIGUSR2, &action, NULL) == -1) {
            fail("sigaction");
        }

        while (handled < 4) {
            struct KeyMessage message;

            if (!photo_interrupt) {
                pause();
            }
            photo_interrupt = 0;

            if (msgrcv(queue_id, &message, sizeof(message) - sizeof(long), 2, 0) == -1) {
                fail("msgrcv");
            }

            if (strcmp(message.key, "-->") == 0) {
                printf("Photo frame process: next photo\n");
            } else if (strcmp(message.key, "<--") == 0) {
                printf("Photo frame process: previous photo\n");
            } else if (strcmp(message.key, "backspace") == 0) {
                printf("Photo frame process: delete current photo\n");
            } else {
                printf("Photo frame process: exit command received\n");
            }
            handled++;
        }

        _exit(EXIT_SUCCESS);
    }

    key_pid = fork();
    if (key_pid == -1) {
        fail("fork");
    }
    if (key_pid == 0) {
        size_t index;

        /* This child generates a small demo key sequence. */
        for (index = 0; index < sizeof(keys) / sizeof(keys[0]); ++index) {
            struct KeyMessage message;

            if (strcmp(keys[index], "^") == 0 || strcmp(keys[index], "v") == 0) {
                message.mtype = 1;
                strcpy(message.key, keys[index]);
                if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                    fail("msgsnd");
                }
                kill(audio_pid, SIGUSR1);
            } else {
                message.mtype = 2;
                strcpy(message.key, keys[index]);
                if (msgsnd(queue_id, &message, sizeof(message) - sizeof(long), 0) == -1) {
                    fail("msgsnd");
                }
                kill(photo_pid, SIGUSR2);
            }

            printf("Key input process: key %s generated\n", keys[index]);
            sleep_ms(220);
        }

        {
            struct KeyMessage stop_audio;
            struct KeyMessage stop_photo;

            stop_audio.mtype = 1;
            strcpy(stop_audio.key, "stop");
            msgsnd(queue_id, &stop_audio, sizeof(stop_audio) - sizeof(long), 0);
            kill(audio_pid, SIGUSR1);

            stop_photo.mtype = 2;
            strcpy(stop_photo.key, "stop");
            msgsnd(queue_id, &stop_photo, sizeof(stop_photo) - sizeof(long), 0);
            kill(photo_pid, SIGUSR2);
        }

        _exit(EXIT_SUCCESS);
    }

    waitpid(key_pid, NULL, 0);
    waitpid(audio_pid, NULL, 0);
    waitpid(photo_pid, NULL, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
