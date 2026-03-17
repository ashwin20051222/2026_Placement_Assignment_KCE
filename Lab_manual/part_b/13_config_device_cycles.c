#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Step {
    char device[32];
    int on_ms;
    int off_ms;
    int cycles;
};

int parse_line(const char *line, struct Step *s) {

    return sscanf(line, "%31s %d %d %d", s->device, &s->on_ms, &s->off_ms, &s->cycles) == 4;
}

int main() {
    const char *cfg = "device_cycles.conf";
    FILE *fp = fopen(cfg, "r");
    if (!fp) {
        perror("fopen");
        puts("Create a config file named device_cycles.conf with lines like:");
        puts("pump 200 300 3");
        puts("door 100 100 2");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        struct Step s;
        if (!parse_line(line, &s)) continue;
        printf("Device %s cycles=%d (on=%dms off=%dms)\n", s.device, s.cycles, s.on_ms, s.off_ms);
        for (int i = 0; i < s.cycles; i++) {
            printf("  %s: ON\n", s.device);
            usleep((useconds_t)(s.on_ms * 1000));
            printf("  %s: OFF\n", s.device);
            usleep((useconds_t)(s.off_ms * 1000));
        }
    }
    fclose(fp);
    return 0;
}
