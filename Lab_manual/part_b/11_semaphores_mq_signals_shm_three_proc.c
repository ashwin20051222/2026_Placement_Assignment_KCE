#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

union semun { int val; };

struct SharedCounter {
    int count;
};

struct Msg {
    long mtype;
    int  value;
};

void sem_wait_op(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op  = -1;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

void sem_signal_op(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op  = +1;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

void producer(int shmid, int semid, int qid) {
    struct SharedCounter *s = (struct SharedCounter *)shmat(shmid, NULL, 0);
    struct Msg m;
    int i;
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    for (i = 1; i <= 8; i++) {
        int value = rand() % 100;
        sem_wait_op(semid);
        s->count += value;
        printf("[producer] Added %d -> count=%d\n", value, s->count);
        sem_signal_op(semid);

        m.mtype = 1;
        m.value = value;
        msgsnd(qid, &m, sizeof(int), 0);
        usleep(200000);
    }
    shmdt(s);
}

void consumer(int qid) {
    struct Msg m;
    int i;
    printf("[consumer] Consumer started.\n");
    for (i = 0; i < 8; i++) {
        if (msgrcv(qid, &m, sizeof(int), 1, 0) < 0) break;
        printf("[consumer] Received value=%d\n", m.value);
    }
}

int main() {

    int shmid = shmget(IPC_PRIVATE, sizeof(struct SharedCounter), IPC_CREAT | 0600);
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    int qid   = msgget(IPC_PRIVATE, IPC_CREAT | 0600);

    if (shmid < 0 || semid < 0 || qid < 0) {
        perror("IPC create"); return 1;
    }

    struct SharedCounter *s = (struct SharedCounter *)shmat(shmid, NULL, 0);
    s->count = 0;
    shmdt(s);

    union semun su;
    su.val = 1;
    semctl(semid, 0, SETVAL, su);

    pid_t prod_pid = fork();
    if (prod_pid == 0) { producer(shmid, semid, qid); exit(0); }

    pid_t cons_pid = fork();
    if (cons_pid == 0) { consumer(qid);               exit(0); }

    s = (struct SharedCounter *)shmat(shmid, NULL, 0);
    int i;
    for (i = 0; i < 5; i++) {
        sem_wait_op(semid);
        printf("[monitor ] Shared count = %d\n", s->count);
        sem_signal_op(semid);
        usleep(350000);
    }
    shmdt(s);

    waitpid(prod_pid, NULL, 0);
    waitpid(cons_pid, NULL, 0);

    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    msgctl(qid,   IPC_RMID, NULL);
    printf("[main  ] All done.\n");
    return 0;
}
