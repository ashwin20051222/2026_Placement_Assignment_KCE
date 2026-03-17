#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>

struct SharedData {
    pthread_mutex_t mutex;
    char time_str[32];
    int  temperature_c;
    int  humidity_percent;
};

void init_shared(struct SharedData *s) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&s->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

void time_reader(struct SharedData *s) {
    int i;
    for (i = 0; i < 10; i++) {
        time_t now = time(NULL);
        struct tm t;
        localtime_r(&now, &t);
        char buf[32];
        strftime(buf, sizeof(buf), "%H:%M:%S", &t);

        pthread_mutex_lock(&s->mutex);
        strcpy(s->time_str, buf);
        pthread_mutex_unlock(&s->mutex);

        usleep(200000);
    }
}

void temp_reader(struct SharedData *s) {
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 10; i++) {
        int temp = 18 + (rand() % 16);
        pthread_mutex_lock(&s->mutex);
        s->temperature_c = temp;
        pthread_mutex_unlock(&s->mutex);
        usleep(220000);
    }
}

void humidity_reader(struct SharedData *s) {
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 0; i < 10; i++) {
        int hum = 30 + (rand() % 41);
        pthread_mutex_lock(&s->mutex);
        s->humidity_percent = hum;
        pthread_mutex_unlock(&s->mutex);
        usleep(240000);
    }
}

void display(struct SharedData *s) {
    int i;
    for (i = 0; i < 10; i++) {
        pthread_mutex_lock(&s->mutex);
        printf("[display] Time=%-8s  Temp=%d C  Humidity=%d%%\n",
               s->time_str, s->temperature_c, s->humidity_percent);
        pthread_mutex_unlock(&s->mutex);
        usleep(250000);
    }
}

int main() {

    int shmid = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0600);
    if (shmid < 0) { perror("shmget"); return 1; }

    struct SharedData *s = (struct SharedData *)shmat(shmid, NULL, 0);
    if (s == (void *)-1) { perror("shmat"); return 1; }
    memset(s, 0, sizeof(*s));
    init_shared(s);

    printf("=== Shared Memory Demo (Time, Temp, Humidity) ===\n");

    pid_t p1 = fork(); if (p1 == 0) { time_reader(s);     exit(0); }
    pid_t p2 = fork(); if (p2 == 0) { temp_reader(s);     exit(0); }
    pid_t p3 = fork(); if (p3 == 0) { humidity_reader(s); exit(0); }
    pid_t p4 = fork(); if (p4 == 0) { display(s);         exit(0); }

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);
    waitpid(p4, NULL, 0);

    shmdt(s);
    shmctl(shmid, IPC_RMID, NULL);
    printf("All processes finished.\n");
    return 0;
}
