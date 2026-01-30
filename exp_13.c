// Priority Schedulling Simulation
// schedprio

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"
#define E_DUPPID "E_DUPPID"

typedef struct {
    char pid[32];
    int arrival;
    int burst;
    int priority;
    int completed;
    int end_time;
} Process;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main() {
    char line[256];
    Process p[100];
    int count = 0;

    // Header
    fgets(line, sizeof(line), stdin);

    while (fgets(line, sizeof(line), stdin)) {
        char *pid = strtok(line, ", \n");
        char *arr = strtok(NULL, ", \n");
        char *bst = strtok(NULL, ", \n");
        char *prio = strtok(NULL, ", \n");
        
        if (!pid || !arr || !bst || !prio) print_error(E_INPUT, "malformed");

        strncpy(p[count].pid, pid, 31);
        p[count].arrival = atoi(arr);
        p[count].burst = atoi(bst);
        p[count].priority = atoi(prio);
        p[count].completed = 0;

        if (p[count].priority < 0 || p[count].priority > 99) print_error(E_RANGE, "priority 0..99");
        count++;
    }

    // Check duplicates
    for(int i=0; i<count; i++) 
        for(int j=i+1; j<count; j++) 
            if(strcmp(p[i].pid, p[j].pid)==0) print_error(E_DUPPID, "dup pid");

    printf("ALG PRIO AGING\n");
    printf("GANTT");

    int current_time = 0;
    int completed = 0;

    while (completed < count) {
        int idx = -1;
        int best_prio = 1000000;

        // Find available processes
        int available_count = 0;
        for (int i = 0; i < count; i++) {
            if (!p[i].completed && p[i].arrival <= current_time) {
                available_count++;
                
                // Calculate Effective Priority
                int wait_time = current_time - p[i].arrival;
                int eff_prio = p[i].priority - wait_time;
                if (eff_prio < 0) eff_prio = 0;

                if (eff_prio < best_prio) {
                    best_prio = eff_prio;
                    idx = i;
                } else if (eff_prio == best_prio) {
                    // Tie-break
                    if (p[i].arrival < p[idx].arrival) idx = i;
                    else if (p[i].arrival == p[idx].arrival && strcmp(p[i].pid, p[idx].pid) < 0) idx = i;
                }
            }
        }

        if (idx != -1) {
            printf(" %s@%d-%d", p[idx].pid, current_time, current_time + p[idx].burst);
            current_time += p[idx].burst;
            p[idx].end_time = current_time;
            p[idx].completed = 1;
            completed++;
        } else {
            // IDLE
             int next_arr = 1000000;
            for(int i=0; i<count; i++) {
                if (!p[i].completed && p[i].arrival < next_arr) 
                    next_arr = p[i].arrival;
            }
            if (next_arr < 1000000) {
                printf(" IDLE@%d-%d", current_time, next_arr);
                current_time = next_arr;
            }
        }
    }
    printf("\n");

    double total_wait = 0, total_tat = 0;
    for (int j = 0; j < count; j++) {
        int tat = p[j].end_time - p[j].arrival;
        int wait = tat - p[j].burst;
        total_tat += tat;
        total_wait += wait;
    }
    printf("OK: AVG_WAIT %.2f AVG_TAT %.2f\n", total_wait / count, total_tat / count);

    return 0;
}