
/*
 * Program 13: Thread-based shared data logger using a mutex.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct SharedState {
    pthread_mutex_t mutex;
    int runtime_minutes;
    int charging_hours;
    int battery_usage_hours;
    char sensor1_event[64];
    char sensor2_event[64];
    int stop;
};

static struct SharedState state = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .runtime_minutes = 0,
    .charging_hours = 0,
    .battery_usage_hours = 0,
    .sensor1_event = "sensor1: none",
    .sensor2_event = "sensor2: none",
    .stop = 0
};

static void sleep_ms(long milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void timestamp_text(char *buffer, size_t size, const char *label) {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char time_text[32];

    if (tm_now == NULL) {
        snprintf(buffer, size, "%s", label);
        return;
    }

    strftime(time_text, sizeof(time_text), "%Y-%m-%d %H:%M:%S", tm_now);
    snprintf(buffer, size, "%s at %s", label, time_text);
}

static void *runtime_thread(void *arg) {
    (void)arg;

    /* Update total runtime every short interval. */
    for (int i = 0; i < 6; ++i) {
        pthread_mutex_lock(&state.mutex);
        state.runtime_minutes += 5;
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(140);
    }
    return NULL;
}

static void *charging_thread(void *arg) {
    (void)arg;

    for (int i = 0; i < 4; ++i) {
        pthread_mutex_lock(&state.mutex);
        state.charging_hours += 1;
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(210);
    }
    return NULL;
}

static void *battery_thread(void *arg) {
    (void)arg;

    for (int i = 0; i < 4; ++i) {
        pthread_mutex_lock(&state.mutex);
        state.battery_usage_hours += 2;
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(190);
    }
    return NULL;
}

static void *sensor1_thread(void *arg) {
    (void)arg;

    for (int i = 0; i < 3; ++i) {
        char event[64];

        timestamp_text(event, sizeof(event), "sensor1 event detected");
        pthread_mutex_lock(&state.mutex);
        strncpy(state.sensor1_event, event, sizeof(state.sensor1_event) - 1);
        state.sensor1_event[sizeof(state.sensor1_event) - 1] = '\0';
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(230);
    }
    return NULL;
}

static void *sensor2_thread(void *arg) {
    (void)arg;

    for (int i = 0; i < 3; ++i) {
        char event[64];

        timestamp_text(event, sizeof(event), "threshold exceeded");
        pthread_mutex_lock(&state.mutex);
        strncpy(state.sensor2_event, event, sizeof(state.sensor2_event) - 1);
        state.sensor2_event[sizeof(state.sensor2_event) - 1] = '\0';
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(260);
    }
    return NULL;
}

static void *event_log_thread(void *arg) {
    (void)arg;

    /* Keep writing the current shared values into the log file. */
    FILE *log_file = fopen("event_log.txt", "w");
    if (log_file == NULL) {
        perror("fopen");
        return NULL;
    }

    for (int i = 0; i < 5; ++i) {
        pthread_mutex_lock(&state.mutex);
        fprintf(log_file,
                "runtime=%d min, charging=%d h, battery=%d h, %s, %s\n",
                state.runtime_minutes,
                state.charging_hours,
                state.battery_usage_hours,
                state.sensor1_event,
                state.sensor2_event);
        fflush(log_file);
        pthread_mutex_unlock(&state.mutex);
        sleep_ms(250);
    }

    fclose(log_file);

    pthread_mutex_lock(&state.mutex);
    state.stop = 1;
    pthread_mutex_unlock(&state.mutex);

    return NULL;
}

static void *display_thread(void *arg) {
    (void)arg;

    /* Wait until logging finishes, then print the file contents. */
    while (1) {
        int should_stop;

        pthread_mutex_lock(&state.mutex);
        should_stop = state.stop;
        pthread_mutex_unlock(&state.mutex);

        if (should_stop) {
            FILE *log_file = fopen("event_log.txt", "r");
            char line[256];

            if (log_file == NULL) {
                perror("fopen");
                return NULL;
            }

            printf("Display thread: showing event log on user request\n");
            while (fgets(line, sizeof(line), log_file) != NULL) {
                fputs(line, stdout);
            }
            fclose(log_file);
            break;
        }

        sleep_ms(120);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[7];

    setbuf(stdout, NULL);

    /* Create all worker threads one by one. */
    pthread_create(&threads[0], NULL, runtime_thread, NULL);
    pthread_create(&threads[1], NULL, charging_thread, NULL);
    pthread_create(&threads[2], NULL, battery_thread, NULL);
    pthread_create(&threads[3], NULL, sensor1_thread, NULL);
    pthread_create(&threads[4], NULL, sensor2_thread, NULL);
    pthread_create(&threads[5], NULL, event_log_thread, NULL);
    pthread_create(&threads[6], NULL, display_thread, NULL);

    /* Wait for every thread to finish before closing the program. */
    for (int i = 0; i < 7; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
