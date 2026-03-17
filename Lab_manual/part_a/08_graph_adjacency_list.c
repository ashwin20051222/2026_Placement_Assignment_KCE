#include <stdio.h>
#include <stdlib.h>

enum {
    MAX_V = 50
};

struct AdjNode {
    int vertex;
    struct AdjNode *next;
};

struct AdjNode *adj[MAX_V];
int num_vertices = 0;

struct AdjNode *adj_new(int v) {
    struct AdjNode *n = (struct AdjNode *)malloc(sizeof(struct AdjNode));
    if (n == NULL) return NULL;
    n->vertex = v;
    n->next   = NULL;
    return n;
}

void add_edge(int u, int v, int undirected) {

    struct AdjNode *a = adj_new(v);
    if (a) { a->next = adj[u]; adj[u] = a; }

    if (undirected) {
        struct AdjNode *b = adj_new(u);
        if (b) { b->next = adj[v]; adj[v] = b; }
    }
}

void print_graph() {
    int i;
    printf("\n--- Adjacency List ---\n");
    for (i = 0; i < num_vertices; i++) {
        printf("Vertex %d: ", i);
        struct AdjNode *p = adj[i];
        while (p != NULL) {
            printf("%d ", p->vertex);
            p = p->next;
        }
        printf("\n");
    }
}

void bfs(int start) {
    int visited[MAX_V] = {0};
    int queue[MAX_V];
    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++]  = start;

    printf("BFS from %d: ", start);
    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);
        struct AdjNode *p = adj[u];
        while (p != NULL) {
            if (!visited[p->vertex]) {
                visited[p->vertex] = 1;
                queue[rear++] = p->vertex;
            }
            p = p->next;
        }
    }
    printf("\n");
}

void dfs_helper(int u, int visited[]) {
    visited[u] = 1;
    printf("%d ", u);
    struct AdjNode *p = adj[u];
    while (p != NULL) {
        if (!visited[p->vertex])
            dfs_helper(p->vertex, visited);
        p = p->next;
    }
}

void dfs(int start) {
    int visited[MAX_V] = {0};
    printf("DFS from %d: ", start);
    dfs_helper(start, visited);
    printf("\n");
}

void free_graph() {
    int i;
    for (i = 0; i < num_vertices; i++) {
        struct AdjNode *p = adj[i];
        while (p != NULL) {
            struct AdjNode *next = p->next;
            free(p);
            p = next;
        }
        adj[i] = NULL;
    }
}

int main() {
    int i, m, undirected, u, v, start;

    printf("=== Graph using Adjacency List ===\n");
    printf("Enter number of vertices: ");
    scanf("%d", &num_vertices);
    if (num_vertices <= 0 || num_vertices > MAX_V) {
        printf("Invalid vertex count.\n"); return 1;
    }

    for (i = 0; i < num_vertices; i++) adj[i] = NULL;

    printf("Undirected graph? (1=yes / 0=no): ");
    scanf("%d", &undirected);
    printf("Enter number of edges: ");
    scanf("%d", &m);

    for (i = 0; i < m; i++) {
        printf("Edge %d (u v): ", i + 1);
        scanf("%d %d", &u, &v);
        if (u < 0 || u >= num_vertices || v < 0 || v >= num_vertices) {
            printf("Invalid vertices. Skipping.\n"); i--; continue;
        }
        add_edge(u, v, undirected);
    }

    print_graph();

    printf("Enter start vertex for BFS and DFS: ");
    scanf("%d", &start);
    if (start >= 0 && start < num_vertices) {
        bfs(start);
        dfs(start);
    }

    free_graph();
    return 0;
}
