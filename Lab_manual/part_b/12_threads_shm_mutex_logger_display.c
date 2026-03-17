#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

enum {
    LOG_LINES = 20,
    LINE_LEN = 128
};

char log_buffer[LOG_LINES][LINE_LEN];
int  log_count = 0;
int  display_index = 0;

pthread_mutex_t log_mutex;

void *logger_thread(void *arg) {
    int i;
    for (i = 1; i <= 10; i++) {

        time_t now = time(NULL);
        struct tm t;
        localtime_r(&now, &t);
        char timebuf[32];
        strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &t);

        pthread_mutex_lock(&log_mutex);
        if (log_count < LOG_LINES) {
            snprintf(log_buffer[log_count], LINE_LEN,
                     "[%s] Log entry #%d: sensor_value=%d", timebuf, i, i * 10);
            log_count++;
        }
        pthread_mutex_unlock(&log_mutex);

        printf("[logger ] Added entry #%d\n", i);
        usleep(300000);
    }
    return NULL;
}

void *display_thread(void *arg) {
    int shown = 0;
    while (shown < 10) {
        pthread_mutex_lock(&log_mutex);
        while (display_index < log_count) {
            printf("[display] %s\n", log_buffer[display_index]);
            display_index++;
            shown++;
        }
        pthread_mutex_unlock(&log_mutex);
        usleep(200000);
    }
    return NULL;
}

void *stats_thread(void *arg) {
    int i;
    for (i = 0; i < 5; i++) {
        usleep(600000);
        pthread_mutex_lock(&log_mutex);
        printf("[stats  ] Total log entries so far: %d\n", log_count);
        pthread_mutex_unlock(&log_mutex);
    }
    return NULL;
}

int main() {
    pthread_t logger_tid, display_tid, stats_tid;

    printf("=== Threads with Mutex (Logger + Display) ===\n");

    pthread_mutex_init(&log_mutex, NULL);

    pthread_create(&logger_tid,  NULL, logger_thread,  NULL);
    pthread_create(&display_tid, NULL, display_thread, NULL);
    pthread_create(&stats_tid,   NULL, stats_thread,   NULL);

    pthread_join(logger_tid,  NULL);
    pthread_join(display_tid, NULL);
    pthread_join(stats_tid,   NULL);

    pthread_mutex_destroy(&log_mutex);
    printf("All threads done. Total entries: %d\n", log_count);
    return 0;
}
