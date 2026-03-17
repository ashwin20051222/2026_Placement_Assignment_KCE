#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Record {
    int  id;
    char name[64];
    int  value;
};

int compare_records(const void *a, const void *b) {
    struct Record *ra = (struct Record *)a;
    struct Record *rb = (struct Record *)b;
    if (ra->value != rb->value)
        return (ra->value < rb->value) ? -1 : 1;
    return (ra->id < rb->id) ? -1 : (ra->id > rb->id);
}

int main() {
    char response[16];
    int  n, i;

    printf("=== Serial Handshake + File Write + Sort ===\n");
    printf("Handshake: type 'OK' to continue: ");
    fgets(response, sizeof(response), stdin);
    response[strcspn(response, "\n")] = 0;

    if (strncmp(response, "OK", 2) != 0) {
        printf("Handshake failed. Exiting.\n");
        return 1;
    }
    printf("Handshake successful!\n");

    printf("Enter number of records: ");
    scanf("%d", &n);
    if (n <= 0) return 1;

    struct Record *recs = (struct Record *)malloc(n * sizeof(struct Record));
    if (recs == NULL) { printf("Memory error.\n"); return 1; }

    for (i = 0; i < n; i++) {
        printf("Record %d - ID Name Value: ", i + 1);
        scanf("%d %63s %d", &recs[i].id, recs[i].name, &recs[i].value);
    }

    FILE *fp = fopen("records.txt", "w");
    if (fp == NULL) { perror("fopen"); free(recs); return 1; }
    for (i = 0; i < n; i++)
        fprintf(fp, "%d %s %d\n", recs[i].id, recs[i].name, recs[i].value);
    fclose(fp);
    printf("Wrote %d records to records.txt\n", n);

    qsort(recs, n, sizeof(struct Record), compare_records);

    printf("\nSorted records (by value):\n");
    for (i = 0; i < n; i++)
        printf("  ID=%d  Name=%-10s  Value=%d\n", recs[i].id, recs[i].name, recs[i].value);

    free(recs);
    return 0;
}
