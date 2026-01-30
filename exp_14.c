// ##########  Deadlock Avoidance Using Bankers Algortihm #######
// bankers

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"
#define E_INVALID "E_INVALID"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main() {
    int P, R;
    if (scanf("%d %d", &P, &R) != 2) return 0; // Empty input handling

    if (P < 1 || R < 1) print_error(E_INPUT, "P and R must be positive");

    int alloc[P][R];
    int max[P][R];
    int avail[R];
    int need[P][R];

    // Read Allocation
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            if (scanf("%d", &alloc[i][j]) != 1) print_error(E_INPUT, "missing data");
            if (alloc[i][j] < 0) print_error(E_RANGE, "negative value");
        }
    }

    // Read Max
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            if (scanf("%d", &max[i][j]) != 1) print_error(E_INPUT, "missing data");
            if (max[i][j] < 0) print_error(E_RANGE, "negative value");
            
            // Check Alloc <= Max
            if (alloc[i][j] > max[i][j]) {
                print_error(E_INVALID, "allocation must be <= max");
            }
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    for (int j = 0; j < R; j++) {
        if (scanf("%d", &avail[j]) != 1) print_error(E_INPUT, "missing data");
        if (avail[j] < 0) print_error(E_RANGE, "negative value");
    }

    // Banker's Algorithm
    int work[R];
    for(int j=0; j<R; j++) work[j] = avail[j];

    bool finish[P];
    for(int i=0; i<P; i++) finish[i] = false;

    int seq[P];
    int seq_idx = 0;

    int completed = 0;
    while (completed < P) {
        bool found = false;
        
        // Iterate 0..P-1 to ensure lexicographically smallest sequence
        for (int i = 0; i < P; i++) {
            if (!finish[i]) {
                bool possible = true;
                for (int j = 0; j < R; j++) {
                    if (need[i][j] > work[j]) {
                        possible = false;
                        break;
                    }
                }

                if (possible) {
                    // Execute process i
                    for (int j = 0; j < R; j++) {
                        work[j] += alloc[i][j];
                    }
                    finish[i] = true;
                    seq[seq_idx++] = i;
                    completed++;
                    found = true;
                    // Restart search from process 0 to maintain lexicographical order requirement
                    break; 
                }
            }
        }

        if (!found) {
            break; // Deadlock/Unsafe
        }
    }

    if (completed == P) {
        printf("OK: SAFE\n");
        printf("OK: SEQ");
        for (int i = 0; i < P; i++) {
            printf(" %d", seq[i]);
        }
        printf("\n");
    } else {
        printf("OK: UNSAFE\n");
    }

    return 0;
}