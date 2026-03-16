
/*
 * Program 10: Simple media capture and display file demo.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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

static void preview_file(const char *path) {
    FILE *file = fopen(path, "rb");
    unsigned char buffer[24];
    size_t bytes_read;
    size_t index;

    if (file == NULL) {
        fail("fopen");
    }

    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    printf("Display process: preview of %s (%zu bytes): ", path, bytes_read);
    for (index = 0; index < bytes_read; ++index) {
        printf("%02X ", buffer[index]);
    }
    printf("\n");
}

int main(void) {
    pid_t image_pid;
    pid_t video_pid;
    int choice = 1;

    setbuf(stdout, NULL);

    image_pid = fork();
    if (image_pid == -1) {
        fail("fork");
    }
    if (image_pid == 0) {
        FILE *image = fopen("captured_image.jpg", "wb");
        const unsigned char jpeg_data[] = {
            0xFF, 0xD8, 0xFF, 0xE0, 'S', 'I', 'M', 'G', 'P', 'H', 'O', 'T', 'O', 0xFF, 0xD9
        };

        if (image == NULL) {
            fail("fopen");
        }

        fwrite(jpeg_data, 1, sizeof(jpeg_data), image);
        fclose(image);
        printf("Image process: captured_image.jpg created\n");
        _exit(EXIT_SUCCESS);
    }

    video_pid = fork();
    if (video_pid == -1) {
        fail("fork");
    }
    if (video_pid == 0) {
        FILE *video = fopen("recorded_video.3gp", "wb");
        const char *content = "SIMULATED_3GP_VIDEO_STREAM_FRAME_01";

        if (video == NULL) {
            fail("fopen");
        }

        fwrite(content, 1, strlen(content), video);
        fclose(video);
        printf("Video process: recorded_video.3gp created\n");
        _exit(EXIT_SUCCESS);
    }

    waitpid(image_pid, NULL, 0);
    waitpid(video_pid, NULL, 0);

    printf("Choose media to display: 1=jpg, 2=3gp, 3=both. Default is 1: ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }

    sleep_ms(150);

    if (choice == 1) {
        preview_file("captured_image.jpg");
    } else if (choice == 2) {
        preview_file("recorded_video.3gp");
    } else {
        preview_file("captured_image.jpg");
        preview_file("recorded_video.3gp");
    }

    return 0;
}
