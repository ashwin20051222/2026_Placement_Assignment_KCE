#include <stdio.h>

#define MAX 20

typedef struct {
    int row;
    int col;
    int dir;
} Cell;

static int is_valid(int rows, int cols, int row, int col) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

int main(void) {
    int rows;
    int cols;
    int maze[MAX][MAX];
    int visited[MAX][MAX] = {{0}};
    Cell stack[MAX * MAX];
    int top = -1;
    int sr;
    int sc;
    int er;
    int ec;
    int found = 0;
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};

    printf("Enter number of rows and columns (max %d): ", MAX);
    scanf("%d %d", &rows, &cols);
    if (rows <= 0 || cols <= 0 || rows > MAX || cols > MAX) {
        printf("Invalid maze size.\n");
        return 0;
    }

    printf("Enter maze matrix (0 for open path, 1 for wall):\n");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            scanf("%d", &maze[i][j]);
        }
    }

    printf("Enter start cell (row col): ");
    scanf("%d %d", &sr, &sc);
    printf("Enter end cell (row col): ");
    scanf("%d %d", &er, &ec);

    if (!is_valid(rows, cols, sr, sc) || !is_valid(rows, cols, er, ec) ||
        maze[sr][sc] == 1 || maze[er][ec] == 1) {
        printf("Invalid start or end cell.\n");
        return 0;
    }

    stack[++top] = (Cell){sr, sc, 0};
    visited[sr][sc] = 1;

    while (top >= 0) {
        Cell *current = &stack[top];

        if (current->row == er && current->col == ec) {
            found = 1;
            break;
        }

        int moved = 0;
        while (current->dir < 4) {
            int direction = current->dir++;
            int nr = current->row + dr[direction];
            int nc = current->col + dc[direction];

            if (is_valid(rows, cols, nr, nc) && maze[nr][nc] == 0 && !visited[nr][nc]) {
                visited[nr][nc] = 1;
                stack[++top] = (Cell){nr, nc, 0};
                moved = 1;
                break;
            }
        }

        if (!moved) {
            --top;
        }
    }

    if (!found) {
        printf("No path found through the maze.\n");
        return 0;
    }

    printf("Path found using stack backtracking:\n");
    for (int i = 0; i <= top; ++i) {
        printf("(%d,%d)", stack[i].row, stack[i].col);
        if (i < top) {
            printf(" -> ");
        }
    }
    printf("\n");

    return 0;
}
