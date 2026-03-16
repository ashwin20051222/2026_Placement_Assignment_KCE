#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 5000
#define MAX_WORD_LEN 64

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordEntry;

static int compare_frequency(const void *a, const void *b) {
    const WordEntry *left = (const WordEntry *) a;
    const WordEntry *right = (const WordEntry *) b;

    if (right->count != left->count) {
        return right->count - left->count;
    }
    return strcmp(left->word, right->word);
}

static void add_word(WordEntry entries[], int *size, const char *word) {
    for (int i = 0; i < *size; ++i) {
        if (strcmp(entries[i].word, word) == 0) {
            ++entries[i].count;
            return;
        }
    }

    if (*size >= MAX_WORDS) {
        return;
    }

    strncpy(entries[*size].word, word, MAX_WORD_LEN - 1);
    entries[*size].word[MAX_WORD_LEN - 1] = '\0';
    entries[*size].count = 1;
    ++(*size);
}

int main(void) {
    char path[256];
    FILE *file;
    WordEntry entries[MAX_WORDS];
    int entry_count = 0;
    char word[MAX_WORD_LEN];
    int length = 0;
    int ch;

    printf("Enter file path: ");
    if (fgets(path, sizeof(path), stdin) == NULL) {
        return 0;
    }
    path[strcspn(path, "\n")] = '\0';

    file = fopen(path, "r");
    if (file == NULL) {
        printf("Unable to open file.\n");
        return 0;
    }

    while ((ch = fgetc(file)) != EOF) {
        if (isalnum((unsigned char) ch)) {
            if (length < MAX_WORD_LEN - 1) {
                word[length++] = (char) tolower((unsigned char) ch);
            }
        } else if (length > 0) {
            word[length] = '\0';
            add_word(entries, &entry_count, word);
            length = 0;
        }
    }

    if (length > 0) {
        word[length] = '\0';
        add_word(entries, &entry_count, word);
    }

    fclose(file);

    if (entry_count == 0) {
        printf("No words found in the file.\n");
        return 0;
    }

    qsort(entries, (size_t) entry_count, sizeof(entries[0]), compare_frequency);

    printf("\nMost frequent words:\n");
    int limit = entry_count < 10 ? entry_count : 10;
    for (int i = 0; i < limit; ++i) {
        printf("%-20s %d\n", entries[i].word, entries[i].count);
    }

    return 0;
}
