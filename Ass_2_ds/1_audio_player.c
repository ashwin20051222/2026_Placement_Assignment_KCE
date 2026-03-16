#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct Node {
    char ch[100];
    struct Node *next;
    struct Node *prev;
};

struct Node *head = NULL, *tail = NULL;

void add_Song() {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    printf("Enter the song name:");
    fgets(newNode->ch, 100, stdin);
    for(int i = 0; newNode->ch[i] != '\0'; i++)
    if(newNode->ch[i] == '\n')
    {
        newNode->ch[i] = '\0';
        break;
    }
    newNode -> next = NULL;
    newNode -> prev = NULL;

    if(head == NULL) {
        head = tail = newNode;
    }
    else {
        tail -> next = newNode;
        newNode -> prev = tail;
        tail = newNode;
    }

    printf("%s added to the playlist\n", newNode -> ch);
}

void delete_Song() {
    if(head == NULL) {
        printf("Playlist is empty\n");
        return;
    }
s2:
    printf("\nEnter the name of the song want to delete: ");
    char name[100];
    fgets(name,100,stdin);
    for(int i=0; name[i] != '\0'; i++) {
        if(name[i] == '\n') {
            name[i] = '\0';
            break;
        }
    }
    struct Node *temp = head;
    while(temp != NULL) {

        if(strcmp(temp->ch,name) == 0) {
            if(temp == head) {
                head = temp->next;
                if(head != NULL) {
                    head->prev = NULL;
                }
                else {
                    tail = NULL;
                }
            }

            else if(temp == tail) {

                tail = temp->prev;
                tail->next = NULL;
            }

            else {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }

            printf("%s deleted from the playlist\n", name);
            free(temp);
            return;
        }
        temp = temp->next;
    }
    printf("%s not found in the playlist\n", name);
    printf("Do you want to exit delete operation 1 yes ,0 no: ");
    int n;
    scanf("%d",&n);
    getchar();

    if(n == 0) {
        goto s2;
    }
}

void display_Playlist() {
    if(head == NULL) {
        printf("Playlist is empty\n");
    }
    else {
        struct Node *temp = tail;
        printf("Playlist:\n");
        while(temp != NULL) {
            printf("%s\n", temp -> ch);
            temp = temp -> prev;
        }
    }
}

int main() {    
    int num;
    while(1) {
        s1: printf("\n1) Add song\n2) Delete song\n3) Display Playlist\n4) Exit\n");
        printf("Enter choice: ");
        scanf("%d", &num);
        getchar();

        switch(num) {
            case 1: add_Song();
                    break;
            case 2: delete_Song();
                    break;
            case 3: display_Playlist();
                    break;
            case 4: exit(0);
                    break;
            default: printf("Invalid input\n");
                     goto s1;
        }

    }
    return 0;
}
