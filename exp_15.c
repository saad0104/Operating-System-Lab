// ############  Deadlock Detection Using Wait-for graph  ########
// waitfor

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"

#define MAX_P 1000

int adj[MAX_P][MAX_P];
int P, E;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

// Stack for path reconstruction
int path[MAX_P];
int path_len = 0;
bool visited[MAX_P];

bool dfs(int u, int start_node) {
    path[path_len++] = u;
    visited[u] = true;

    // Check neighbors in increasing order for lexicographical requirement
    for (int v = 0; v < P; v++) {
        if (adj[u][v]) {
            if (v == start_node) {
     // Cycle found back to start!
                return true; 
            }
            if (v > start_node && !visited[v]) {
                if (dfs(v, start_node)) return true;
            }
        }
    }

    // Backtrack
    path_len--;
    
    return false;
}

int main() {
    if (scanf("%d %d", &P, &E) != 2) return 0;
    
    if (P < 0 || E < 0) print_error(E_INPUT, "negative counts");
    if (P > MAX_P) print_error(E_RANGE, "P exceeds limit");

    memset(adj, 0, sizeof(adj));

    for (int i = 0; i < E; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) print_error(E_INPUT, "bad edge");
        if (u < 0 || u >= P || v < 0 || v >= P) print_error(E_RANGE, "node out of range");
        adj[u][v] = 1;
    }

    // canonical cycle check
    for (int i = 0; i < P; i++) {
        // Reset search state
        path_len = 0;
        for(int k=0; k<P; k++) visited[k] = false;

        // cycle check
        if (dfs(i, i)) {
            printf("OK: DEADLOCK YES\n");
            printf("OK: CYCLE");
            for (int k = 0; k < path_len; k++) {
                printf(" %d", path[k]);
            }
            printf(" %d\n", i); 
            return 0;
        }
    }

    printf("OK: DEADLOCK NO\n");
    return 0;
}