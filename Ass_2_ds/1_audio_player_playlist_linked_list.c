#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Song {
    char title[100];
    struct Song *next;
} Song;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int) size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static Song *create_song(const char *title) {
    Song *song = (Song *) malloc(sizeof(Song));
    if (song == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(song->title, title, sizeof(song->title) - 1);
    song->title[sizeof(song->title) - 1] = '\0';
    song->next = NULL;
    return song;
}

static void add_song(Song **head, const char *title) {
    Song *song = create_song(title);
    if (*head == NULL) {
        *head = song;
        return;
    }

    Song *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = song;
}

static void insert_after(Song *head, const char *after_title, const char *new_title) {
    Song *temp = head;
    while (temp != NULL && strcmp(temp->title, after_title) != 0) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Song not found in playlist.\n");
        return;
    }

    Song *song = create_song(new_title);
    song->next = temp->next;
    temp->next = song;
}

static void delete_song(Song **head, const char *title) {
    Song *temp = *head;
    Song *prev = NULL;

    while (temp != NULL && strcmp(temp->title, title) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Song not found.\n");
        return;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
    printf("Song removed from playlist.\n");
}

static void display_playlist(Song *head) {
    if (head == NULL) {
        printf("Playlist is empty.\n");
        return;
    }

    printf("\nPlaylist:\n");
    int index = 1;
    while (head != NULL) {
        printf("%d. %s\n", index++, head->title);
        head = head->next;
    }
}

static void free_playlist(Song *head) {
    while (head != NULL) {
        Song *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    Song *playlist = NULL;
    int choice;
    char title[100];
    char after_title[100];

    while (1) {
        printf("\nAudio Player Playlist Menu\n");
        printf("1. Add song\n");
        printf("2. Insert song after another song\n");
        printf("3. Delete song\n");
        printf("4. Display playlist\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("Enter song title: ");
                read_line(title, sizeof(title));
                add_song(&playlist, title);
                break;
            case 2:
                printf("Enter existing song title: ");
                read_line(after_title, sizeof(after_title));
                printf("Enter new song title: ");
                read_line(title, sizeof(title));
                insert_after(playlist, after_title, title);
                break;
            case 3:
                printf("Enter song title to delete: ");
                read_line(title, sizeof(title));
                delete_song(&playlist, title);
                break;
            case 4:
                display_playlist(playlist);
                break;
            case 5:
                free_playlist(playlist);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_playlist(playlist);
    return 0;
}
