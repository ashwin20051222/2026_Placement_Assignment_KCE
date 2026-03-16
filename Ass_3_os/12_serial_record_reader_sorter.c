
/*
 * Program 12: Serial-style record reader with handshaking and sorting.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Record {
    int id;
    char parameter[32];
    float value;
    int priority;
};

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

static int compare_records(const void *left, const void *right) {
    const struct Record *a = left;
    const struct Record *b = right;

    if (a->priority != b->priority) {
        return b->priority - a->priority;
    }
    return a->id - b->id;
}

int main(void) {
    int serial_fds[2];
    pid_t device_pid;
    FILE *serial_stream;
    FILE *raw_file;
    FILE *sorted_file;
    struct Record records[8];
    int record_count = 0;
    char line[128];

    setbuf(stdout, NULL);

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, serial_fds) == -1) {
        fail("socketpair");
    }

    device_pid = fork();
    if (device_pid == -1) {
        fail("fork");
    }

    if (device_pid == 0) {
        FILE *device_stream = fdopen(serial_fds[1], "r+");

        if (device_stream == NULL) {
            fail("fdopen");
        }

        close(serial_fds[0]);

        if (fgets(line, sizeof(line), device_stream) != NULL && strcmp(line, "HELLO_DEVICE\n") == 0) {
            fprintf(device_stream, "ACK\n");
            fprintf(device_stream, "101,temperature,37.5,2\n");
            fprintf(device_stream, "102,heart_rate,84.0,3\n");
            fprintf(device_stream, "103,spo2,97.0,1\n");
            fprintf(device_stream, "104,bp_systolic,128.0,4\n");
            fflush(device_stream);
        }

        fclose(device_stream);
        _exit(EXIT_SUCCESS);
    }

    close(serial_fds[1]);
    serial_stream = fdopen(serial_fds[0], "r+");
    if (serial_stream == NULL) {
        fail("fdopen");
    }

    fprintf(serial_stream, "HELLO_DEVICE\n");
    fflush(serial_stream);

    if (fgets(line, sizeof(line), serial_stream) == NULL || strcmp(line, "ACK\n") != 0) {
        fprintf(stderr, "Handshake failed\n");
        fclose(serial_stream);
        waitpid(device_pid, NULL, 0);
        return EXIT_FAILURE;
    }

    raw_file = fopen("serial_records.csv", "w");
    if (raw_file == NULL) {
        fail("fopen");
    }

    while (fgets(line, sizeof(line), serial_stream) != NULL && record_count < 8) {
        struct Record *record = &records[record_count];

        if (sscanf(line, "%d,%31[^,],%f,%d",
                   &record->id,
                   record->parameter,
                   &record->value,
                   &record->priority) == 4) {
            fputs(line, raw_file);
            record_count++;
        }
    }

    fclose(raw_file);
    fclose(serial_stream);
    waitpid(device_pid, NULL, 0);

    qsort(records, (size_t)record_count, sizeof(records[0]), compare_records);

    sorted_file = fopen("serial_records_sorted.csv", "w");
    if (sorted_file == NULL) {
        fail("fopen");
    }

    for (int index = 0; index < record_count; ++index) {
        fprintf(sorted_file, "%d,%s,%.1f,%d\n",
                records[index].id,
                records[index].parameter,
                records[index].value,
                records[index].priority);
    }

    fclose(sorted_file);

    printf("Handshake completed, %d records captured in serial_records.csv\n", record_count);
    printf("Sorted output written to serial_records_sorted.csv\n");

    return 0;
}
