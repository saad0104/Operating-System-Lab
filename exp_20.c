// Disk Scheduling Algorithm
// disksched

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"
#define E_DIR "E_DIR"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void solve_fcfs(int *reqs, int L, int start) {
    printf("ALG FCFS\n");
    int moves = 0;
    int cur = start;
    
    printf("OK: ORDER");
    for (int i = 0; i < L; i++) {
        printf(" %d", reqs[i]);
        moves += abs(reqs[i] - cur);
        cur = reqs[i];
    }
    printf("\nOK: MOVES %d\n", moves);
}

void solve_sstf(int *orig_reqs, int L, int start) {
    printf("ALG SSTF\n");
    int *reqs = malloc(sizeof(int) * L);
    bool *done = calloc(L, sizeof(bool));
    memcpy(reqs, orig_reqs, sizeof(int) * L);
    
    int moves = 0;
    int cur = start;
    
    printf("OK: ORDER");
    for (int count = 0; count < L; count++) {
        int best_idx = -1;
        int min_dist = 2000000000;

        for (int i = 0; i < L; i++) {
            if (!done[i]) {
                int dist = abs(reqs[i] - cur);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_idx = i;
                } else if (dist == min_dist) {
                    // Tie-break: choose smaller cylinder number
                    if (reqs[i] < reqs[best_idx]) best_idx = i;
                }
            }
        }

        if (best_idx != -1) {
            printf(" %d", reqs[best_idx]);
            done[best_idx] = true;
            moves += min_dist;
            cur = reqs[best_idx];
        }
    }
    printf("\nOK: MOVES %d\n", moves);
    free(reqs);
    free(done);
}

// Helper for SCAN/C-SCAN
void solve_scan_variants(int *orig_reqs, int L, int start, int max_cyl, const char *dir_str, bool circular) {
    printf("ALG %s\n", circular ? "C-SCAN" : "SCAN");
    
    // Sort requests for easier processing
    int *sorted = malloc(sizeof(int) * L);
    memcpy(sorted, orig_reqs, sizeof(int) * L);
    qsort(sorted, L, sizeof(int), cmp_int);
    
    int moves = 0;
    int cur = start;
    printf("OK: ORDER");

    int direction = (strcmp(dir_str, "right") == 0) ? 1 : -1;

    // Find split point
    int split = 0;
    while (split < L && sorted[split] < start) split++;

    if (direction == 1) { 
        // 1. Go Right (split -> L-1)
        for (int i = split; i < L; i++) {
            printf(" %d", sorted[i]);
            moves += abs(sorted[i] - cur);
            cur = sorted[i];
        }
        
        if (!circular) {
           
             if (cur != max_cyl && split > 0) { 
                 moves += abs(max_cyl - cur);
                 cur = max_cyl;
             }
             // Reverse Left (split-1 -> 0)
             for (int i = split - 1; i >= 0; i--) {
                 printf(" %d", sorted[i]);
                 moves += abs(sorted[i] - cur);
                 cur = sorted[i];
             }
        } else {
            // C-SCAN: Hit MAX
            if (cur != max_cyl) {
                moves += abs(max_cyl - cur);
                cur = max_cyl;
            }
            // Jump to 0
            if (cur != 0) {
                moves += abs(0 - cur); 
                cur = 0;
            }
            // Continue Right (0 -> split-1)
            for (int i = 0; i < split; i++) {
                printf(" %d", sorted[i]);
                moves += abs(sorted[i] - cur);
                cur = sorted[i];
            }
        }
    } else { // LEFT
        // 1. Go Left (split-1 -> 0)
        for (int i = split - 1; i >= 0; i--) {
            printf(" %d", sorted[i]);
            moves += abs(sorted[i] - cur);
            cur = sorted[i];
        }
        
        if (!circular) {
            // SCAN Left: Hit 0
            if (cur != 0 && split < L) {
                moves += abs(0 - cur);
                cur = 0;
            }
            // Reverse Right (split -> L-1)
            for (int i = split; i < L; i++) {
                printf(" %d", sorted[i]);
                moves += abs(sorted[i] - cur);
                cur = sorted[i];
            }
        } else {
            // C-SCAN Left: Hit 0
            if (cur != 0) {
                moves += abs(0 - cur);
                cur = 0;
            }
            // Jump to Max
            if (cur != max_cyl) {
                moves += abs(max_cyl - cur);
                cur = max_cyl;
            }
            for (int i = L - 1; i >= split; i--) {
                printf(" %d", sorted[i]);
                moves += abs(sorted[i] - cur);
                cur = sorted[i];
            }
        }
    }
    printf("\nOK: MOVES %d\n", moves);
    free(sorted);
}

int main(int argc, char *argv[]) {
    int max_cyl = 0;
    int start_cyl = 0;
    char *dir = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--max") == 0) max_cyl = atoi(argv[++i]);
        else if (strcmp(argv[i], "--start") == 0) start_cyl = atoi(argv[++i]);
        else if (strcmp(argv[i], "--dir") == 0) dir = argv[++i];
    }

    if (max_cyl < 1) print_error(E_RANGE, "max must be >= 1");
    if (start_cyl < 0 || start_cyl > max_cyl) print_error(E_RANGE, "start out of range");
    if (!dir || (strcmp(dir, "left") != 0 && strcmp(dir, "right") != 0)) print_error(E_DIR, "dir must be left or right");

    int L;
    if (scanf("%d", &L) != 1) return 0;
    if (L < 0) print_error(E_INPUT, "bad length");

    int *reqs = malloc(sizeof(int) * L);
    for (int i = 0; i < L; i++) {
        if (scanf("%d", &reqs[i]) != 1) print_error(E_INPUT, "bad req");
        if (reqs[i] < 0 || reqs[i] > max_cyl) print_error(E_RANGE, "request cylinders must be in 0..max");
    }

    solve_fcfs(reqs, L, start_cyl);
    solve_sstf(reqs, L, start_cyl);
    solve_scan_variants(reqs, L, start_cyl, max_cyl, dir, false); 
    solve_scan_variants(reqs, L, start_cyl, max_cyl, dir, true);  

    free(reqs);
    return 0;
}