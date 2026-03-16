
/*
 * Program 14: Configuration-file-driven device operation demo.
 * This version keeps the logic simple and uses fixed sample values so it can run
 * without extra hardware or command-line arguments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct CycleConfig {
    char name[32];
    int pump_speed;
    int duration;
    int volume;
    int concentration;
    int flow_rate;
    int door_duration;
    int drain_duration;
};

static void sleep_ms(long milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void create_default_config(const char *path) {
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "cycle_count=3\n");
    fprintf(file, "cycle1_name=Cycle1\n");
    fprintf(file, "cycle1_pump_speed=120\n");
    fprintf(file, "cycle1_duration=3\n");
    fprintf(file, "cycle1_volume=200\n");
    fprintf(file, "cycle1_concentration=55\n");
    fprintf(file, "cycle1_flow_rate=18\n");
    fprintf(file, "cycle1_door_duration=1\n");
    fprintf(file, "cycle1_drain_duration=1\n");
    fprintf(file, "cycle2_name=Cycle2\n");
    fprintf(file, "cycle2_pump_speed=150\n");
    fprintf(file, "cycle2_duration=4\n");
    fprintf(file, "cycle2_volume=240\n");
    fprintf(file, "cycle2_concentration=65\n");
    fprintf(file, "cycle2_flow_rate=20\n");
    fprintf(file, "cycle2_door_duration=1\n");
    fprintf(file, "cycle2_drain_duration=2\n");
    fprintf(file, "cycle3_name=Cycle3\n");
    fprintf(file, "cycle3_pump_speed=100\n");
    fprintf(file, "cycle3_duration=2\n");
    fprintf(file, "cycle3_volume=180\n");
    fprintf(file, "cycle3_concentration=45\n");
    fprintf(file, "cycle3_flow_rate=16\n");
    fprintf(file, "cycle3_door_duration=1\n");
    fprintf(file, "cycle3_drain_duration=1\n");

    fclose(file);
}

int main(void) {
    const char *config_path = "device_config.txt";
    FILE *file;
    struct CycleConfig cycles[15] = {0};
    char line[128];
    int cycle_count = 0;

    setbuf(stdout, NULL);

    if (access(config_path, F_OK) != 0) {
        create_default_config(config_path);
    }

    file = fopen(config_path, "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char key[64];
        char value[64];

        if (sscanf(line, "%63[^=]=%63s", key, value) != 2) {
            continue;
        }

        if (strcmp(key, "cycle_count") == 0) {
            cycle_count = atoi(value);
            continue;
        }

        {
            int cycle_number;
            char field[32];

            if (sscanf(key, "cycle%d_%31s", &cycle_number, field) == 2 &&
                cycle_number >= 1 && cycle_number <= 15) {
                struct CycleConfig *cycle = &cycles[cycle_number - 1];

                if (strcmp(field, "name") == 0) {
                    strncpy(cycle->name, value, sizeof(cycle->name) - 1);
                } else if (strcmp(field, "pump_speed") == 0) {
                    cycle->pump_speed = atoi(value);
                } else if (strcmp(field, "duration") == 0) {
                    cycle->duration = atoi(value);
                } else if (strcmp(field, "volume") == 0) {
                    cycle->volume = atoi(value);
                } else if (strcmp(field, "concentration") == 0) {
                    cycle->concentration = atoi(value);
                } else if (strcmp(field, "flow_rate") == 0) {
                    cycle->flow_rate = atoi(value);
                } else if (strcmp(field, "door_duration") == 0) {
                    cycle->door_duration = atoi(value);
                } else if (strcmp(field, "drain_duration") == 0) {
                    cycle->drain_duration = atoi(value);
                }
            }
        }
    }

    fclose(file);

    printf("Configuration loaded from %s\n", config_path);

    for (int index = 0; index < cycle_count; ++index) {
        struct CycleConfig *cycle = &cycles[index];

        printf("Running %s: pump=%d rpm, duration=%d min, volume=%d ml, concentration=%d %%, flow=%d ml/min\n",
               cycle->name,
               cycle->pump_speed,
               cycle->duration,
               cycle->volume,
               cycle->concentration,
               cycle->flow_rate);
        printf("Door close/open duration=%d min, fluid drain duration=%d min\n",
               cycle->door_duration,
               cycle->drain_duration);
        printf("Device action: pump start -> fluid dispense -> pressure hold -> drain\n");
        sleep_ms(250);
    }

    return 0;
}
