// ######## CPU Scheduling Simulator II(Round Robin) ######
// schedsim2

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
    int remaining;
    int end_time;
    int enqueued; 
} Process;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int quantum = 0;
    if (argc > 2 && strcmp(argv[1], "--q") == 0) {
        quantum = atoi(argv[2]);
    }
    if (quantum < 1 || quantum > 1000) print_error(E_RANGE, "quantum must be in 1..1000");

    char line[256];
    Process p[100];
    int count = 0;

    fgets(line, sizeof(line), stdin);

    while (fgets(line, sizeof(line), stdin)) {
        char *pid = strtok(line, ", \n");
        char *arr = strtok(NULL, ", \n");
        char *bst = strtok(NULL, ", \n");
        if (!pid || !arr || !bst) print_error(E_INPUT, "bad format");

        strncpy(p[count].pid, pid, 31);
        p[count].arrival = atoi(arr);
        p[count].burst = atoi(bst);
        p[count].remaining = p[count].burst;
        p[count].enqueued = 0;
        
        if (p[count].arrival < 0 || p[count].burst <= 0) print_error(E_RANGE, "bad values");
        count++;
    }

    // Sort by arrival
    for(int i=0; i<count; i++) {
        for(int j=i+1; j<count; j++) {
            if(p[i].arrival > p[j].arrival) {
                Process temp = p[i]; p[i] = p[j]; p[j] = temp;
            } else if (p[i].arrival == p[j].arrival && strcmp(p[i].pid, p[j].pid) > 0) {
                Process temp = p[i]; p[i] = p[j]; p[j] = temp;
            }
        }
    }
    // Check duplicates
    for(int i=0; i<count; i++) 
        for(int j=i+1; j<count; j++) 
            if(strcmp(p[i].pid, p[j].pid)==0) print_error(E_DUPPID, "duplicate");

    // Queue Implementation
    int queue[10000];
    int front = 0, rear = 0;
    int current_time = 0;
    int completed = 0;
    int active_idx = -1;

    printf("ALG RR\n");
    printf("GANTT");


    // To handle initial state correctly
    // Add arrivals at t=0
    for(int i=0; i<count; i++) {
        if (p[i].arrival == 0) {
            queue[rear++] = i;
            p[i].enqueued = 1;
        }
    }

    while (completed < count) {
        if (front == rear) {
            // If nothing in queue, jump to next arrival
            int next_arr = 1000000;
            for(int i=0; i<count; i++) {
                if (p[i].remaining > 0 && !p[i].enqueued && p[i].arrival < next_arr) 
                    next_arr = p[i].arrival;
            }
            if (next_arr < 1000000) {
                printf(" IDLE@%d-%d", current_time, next_arr);
                current_time = next_arr;
                // Enqueue items at this new time
                for(int i=0; i<count; i++) {
                    if (p[i].arrival <= current_time && p[i].remaining > 0 && !p[i].enqueued) {
                        queue[rear++] = i;
                        p[i].enqueued = 1;
                    }
                }
            }
        }

        if (front < rear) {
            int idx = queue[front++];
            
            int run_time = (p[idx].remaining < quantum) ? p[idx].remaining : quantum;
            printf(" %s@%d-%d", p[idx].pid, current_time, current_time + run_time);
            
            current_time += run_time;
            p[idx].remaining -= run_time;

            // Check for new arrivals during this execution
            for(int i=0; i<count; i++) {
                if (p[i].arrival > (current_time - run_time) && p[i].arrival <= current_time && p[i].remaining > 0 && !p[i].enqueued) {
                    queue[rear++] = i;
                    p[i].enqueued = 1;
                }
            }

            // If not finished, re-queue
            if (p[idx].remaining > 0) {
                queue[rear++] = idx;
            } else {
                p[idx].end_time = current_time;
                completed++;
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