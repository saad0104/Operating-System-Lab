// Contiguous Memory Allocation Simulator
// memfit

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

typedef struct {
    int id;
    int size;
    int original_size;
} Block;

// Helper to reset blocks to original state between algorithms
void reset_blocks(Block *blocks, Block *orig, int count) {
    for (int i = 0; i < count; i++) {
        blocks[i] = orig[i];
    }
}

void solve(const char *alg_name, Block *blocks, int b_count, int *procs, int p_count) {
    printf("ALG %s\n", alg_name);
    int allocated_count = 0;

    for (int i = 0; i < p_count; i++) {
        int p_size = procs[i];
        int best_idx = -1;

        if (strcmp(alg_name, "FIRST_FIT") == 0) {
            for (int j = 0; j < b_count; j++) {
                if (blocks[j].size >= p_size) {
                    best_idx = j;
                    break;
                }
            }
        } else if (strcmp(alg_name, "BEST_FIT") == 0) {
            int min_waste = 2000000000;
            for (int j = 0; j < b_count; j++) {
                if (blocks[j].size >= p_size) {
                    int waste = blocks[j].size - p_size;
                    if (waste < min_waste) {
                        min_waste = waste;
                        best_idx = j;
                    }
                }
            }
        } else if (strcmp(alg_name, "WORST_FIT") == 0) {
            int max_waste = -1;
            for (int j = 0; j < b_count; j++) {
                if (blocks[j].size >= p_size) {
                    int waste = blocks[j].size - p_size;
                    if (waste > max_waste) {
                        max_waste = waste;
                        best_idx = j;
                    }
                }
            }
        }

        if (best_idx != -1) {
            printf("PROC %d SIZE %d -> BLOCK %d\n", i, p_size, best_idx);
            blocks[best_idx].size -= p_size; // Block shrinks
            allocated_count++;
        } else {
            printf("PROC %d SIZE %d -> FAIL\n", i, p_size);
        }
    }
    printf("OK: ALLOCATED %d/%d\n", allocated_count, p_count);
}

int main() {
    int B, P;
    
    // Read Blocks
    if (scanf("%d", &B) != 1) return 0;
    if (B <= 0) print_error(E_RANGE, "B must be positive"); 

    Block *orig_blocks = malloc(sizeof(Block) * B);
    Block *work_blocks = malloc(sizeof(Block) * B);

    for (int i = 0; i < B; i++) {
        orig_blocks[i].id = i;
        if (scanf("%d", &orig_blocks[i].original_size) != 1) print_error(E_INPUT, "missing block size");
        if (orig_blocks[i].original_size < 0) print_error(E_RANGE, "block size negative");
        orig_blocks[i].size = orig_blocks[i].original_size;
    }

    // Read Processes
    if (scanf("%d", &P) != 1) print_error(E_INPUT, "missing P");
    if (P <= 0) print_error(E_RANGE, "P must be positive"); // Usually >0 check

    int *procs = malloc(sizeof(int) * P);
    for (int i = 0; i < P; i++) {
        if (scanf("%d", &procs[i]) != 1) print_error(E_INPUT, "missing proc size");
        if (procs[i] < 0) print_error(E_RANGE, "proc size negative");
    }

    // Run Simulations
    reset_blocks(work_blocks, orig_blocks, B);
    solve("FIRST_FIT", work_blocks, B, procs, P);

    reset_blocks(work_blocks, orig_blocks, B);
    solve("BEST_FIT", work_blocks, B, procs, P);

    reset_blocks(work_blocks, orig_blocks, B);
    solve("WORST_FIT", work_blocks, B, procs, P);

    free(orig_blocks);
    free(work_blocks);
    free(procs);
    return 0;
}