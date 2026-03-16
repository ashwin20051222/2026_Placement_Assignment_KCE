#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 20

typedef struct AdjNode {
    int dest;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode *head[MAX_VERTICES];
    int vertices;
} Graph;

static AdjNode *create_node(int dest) {
    AdjNode *node = (AdjNode *) malloc(sizeof(AdjNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->dest = dest;
    node->next = NULL;
    return node;
}

static void init_graph(Graph *graph, int vertices) {
    graph->vertices = vertices;
    for (int i = 0; i < MAX_VERTICES; ++i) {
        graph->head[i] = NULL;
    }
}

static void add_edge(Graph *graph, int src, int dest) {
    if (src < 0 || src >= graph->vertices || dest < 0 || dest >= graph->vertices) {
        printf("Invalid vertices.\n");
        return;
    }

    AdjNode *node = create_node(dest);
    node->next = graph->head[src];
    graph->head[src] = node;

    node = create_node(src);
    node->next = graph->head[dest];
    graph->head[dest] = node;
}

static void display_graph(const Graph *graph) {
    for (int i = 0; i < graph->vertices; ++i) {
        printf("%d:", i);
        AdjNode *temp = graph->head[i];
        while (temp != NULL) {
            printf(" %d", temp->dest);
            temp = temp->next;
        }
        printf("\n");
    }
}

static void bfs(const Graph *graph, int start) {
    if (start < 0 || start >= graph->vertices) {
        printf("Invalid start vertex.\n");
        return;
    }

    int visited[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0;
    int rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    printf("BFS traversal: ");
    while (front < rear) {
        int vertex = queue[front++];
        printf("%d ", vertex);

        AdjNode *temp = graph->head[vertex];
        while (temp != NULL) {
            if (!visited[temp->dest]) {
                visited[temp->dest] = 1;
                queue[rear++] = temp->dest;
            }
            temp = temp->next;
        }
    }
    printf("\n");
}

static void dfs_recursive(const Graph *graph, int vertex, int visited[]) {
    visited[vertex] = 1;
    printf("%d ", vertex);

    AdjNode *temp = graph->head[vertex];
    while (temp != NULL) {
        if (!visited[temp->dest]) {
            dfs_recursive(graph, temp->dest, visited);
        }
        temp = temp->next;
    }
}

static void dfs(const Graph *graph, int start) {
    if (start < 0 || start >= graph->vertices) {
        printf("Invalid start vertex.\n");
        return;
    }

    int visited[MAX_VERTICES] = {0};
    printf("DFS traversal: ");
    dfs_recursive(graph, start, visited);
    printf("\n");
}

static void free_graph(Graph *graph) {
    for (int i = 0; i < graph->vertices; ++i) {
        AdjNode *temp = graph->head[i];
        while (temp != NULL) {
            AdjNode *next = temp->next;
            free(temp);
            temp = next;
        }
    }
}

int main(void) {
    Graph graph;
    int vertices;
    int choice;
    int src;
    int dest;
    int start;

    printf("Enter number of vertices (max %d): ", MAX_VERTICES);
    scanf("%d", &vertices);
    if (vertices <= 0 || vertices > MAX_VERTICES) {
        printf("Invalid number of vertices.\n");
        return 0;
    }

    init_graph(&graph, vertices);

    while (1) {
        printf("\nGraph Menu\n");
        printf("1. Add edge\n");
        printf("2. Display adjacency list\n");
        printf("3. BFS traversal\n");
        printf("4. DFS traversal\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter source and destination: ");
                scanf("%d %d", &src, &dest);
                add_edge(&graph, src, dest);
                break;
            case 2:
                display_graph(&graph);
                break;
            case 3:
                printf("Enter start vertex: ");
                scanf("%d", &start);
                bfs(&graph, start);
                break;
            case 4:
                printf("Enter start vertex: ");
                scanf("%d", &start);
                dfs(&graph, start);
                break;
            case 5:
                free_graph(&graph);
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    free_graph(&graph);
    return 0;
}
