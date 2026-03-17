#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Song {
    char title[128];
    char artist[128];
    int  duration_seconds;
    struct Song *next;
    struct Song *prev;
};

struct Song *song_new(char *title, char *artist, int dur) {
    struct Song *s = (struct Song *)malloc(sizeof(struct Song));
    if (s == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    strcpy(s->title, title);
    strcpy(s->artist, artist);
    s->duration_seconds = dur;
    s->next = NULL;
    s->prev = NULL;
    return s;
}

void playlist_append(struct Song **head, struct Song **tail, struct Song *s) {
    if (*head == NULL) {

        *head = s;
        *tail = s;
    } else {

        s->prev = *tail;
        (*tail)->next = s;
        *tail = s;
    }
}

struct Song *playlist_find(struct Song *head, char *title) {
    struct Song *cur = head;
    while (cur != NULL) {
        if (strcmp(cur->title, title) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void playlist_remove(struct Song **head, struct Song **tail, struct Song *s) {
    if (s == NULL) return;

    if (s->prev != NULL)
        s->prev->next = s->next;
    else
        *head = s->next;

    if (s->next != NULL)
        s->next->prev = s->prev;
    else
        *tail = s->prev;

    free(s);
}

void playlist_print(struct Song *head, struct Song *current) {
    printf("\n--- Playlist ---\n");
    if (head == NULL) {
        printf("(empty)\n");
        return;
    }
    int idx = 1;
    struct Song *cur = head;
    while (cur != NULL) {
        char marker = (cur == current) ? '>' : ' ';
        printf("%c %2d. %s - %s (%d sec)\n",
               marker, idx, cur->title, cur->artist, cur->duration_seconds);
        idx++;
        cur = cur->next;
    }
}

void playlist_free(struct Song *head) {
    while (head != NULL) {
        struct Song *next = head->next;
        free(head);
        head = next;
    }
}

int main() {
    struct Song *head    = NULL;
    struct Song *tail    = NULL;
    struct Song *current = NULL;
    int choice;

    printf("=== Audio Player Playlist (Doubly Linked List) ===\n");

    while (1) {
        printf("\n1. Add song\n");
        printf("2. Remove song by title\n");
        printf("3. Set currently playing\n");
        printf("4. Next song\n");
        printf("5. Previous song\n");
        printf("6. Show playlist\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 0) break;

        if (choice == 1) {
            char title[128], artist[128];
            int dur;
            printf("Title  : "); fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            printf("Artist : "); fgets(artist, sizeof(artist), stdin);
            artist[strcspn(artist, "\n")] = 0;
            printf("Duration (seconds): "); scanf("%d", &dur); getchar();

            struct Song *s = song_new(title, artist, dur);
            if (s == NULL) continue;
            playlist_append(&head, &tail, s);
            if (current == NULL) current = head;
            printf("Song added.\n");

        } else if (choice == 2) {
            char title[128];
            printf("Title to remove: "); fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            struct Song *s = playlist_find(head, title);
            if (s == NULL) { printf("Not found.\n"); continue; }
            if (current == s)
                current = (s->next != NULL) ? s->next : s->prev;
            playlist_remove(&head, &tail, s);
            printf("Removed.\n");

        } else if (choice == 3) {
            char title[128];
            printf("Title to play: "); fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            struct Song *s = playlist_find(head, title);
            if (s == NULL) printf("Not found.\n");
            else { current = s; printf("Now playing: %s\n", current->title); }

        } else if (choice == 4) {
            if (current == NULL) printf("Playlist is empty.\n");
            else if (current->next != NULL) { current = current->next; printf("Now: %s\n", current->title); }
            else printf("Already at last song.\n");

        } else if (choice == 5) {
            if (current == NULL) printf("Playlist is empty.\n");
            else if (current->prev != NULL) { current = current->prev; printf("Now: %s\n", current->title); }
            else printf("Already at first song.\n");

        } else if (choice == 6) {
            playlist_print(head, current);

        } else {
            printf("Invalid choice.\n");
        }
    }

    playlist_free(head);
    return 0;
}
