// CPU Scheduling Simulator
//schedsim1

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
    int start_time;
    int end_time;
    int completed;
} Process;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int compare_arrival(const void *a, const void *b) {
    Process *pa = (Process *)a;
    Process *pb = (Process *)b;
    if (pa->arrival != pb->arrival) return pa->arrival - pb->arrival;
    return strcmp(pa->pid, pb->pid);
}

// check duplicates
void check_duplicates(Process *p, int count) {
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(p[i].pid, p[j].pid) == 0) {
                print_error(E_DUPPID, "duplicate pid found");
            }
        }
    }
}
// First Come First Serve
void solve_fcfs(Process *orig, int count) {
    Process p[100];
    memcpy(p, orig, sizeof(Process) * count);
    
    qsort(p, count, sizeof(Process), compare_arrival);

    printf("ALG FCFS\n");
    printf("GANTT");
    
    int current_time = 0;
    int completed = 0;
    int i = 0;
    while (i < count) {
        if (current_time < p[i].arrival) {
            printf(" IDLE@%d-%d", current_time, p[i].arrival);
            current_time = p[i].arrival;
        }
        printf(" %s@%d-%d", p[i].pid, current_time, current_time + p[i].burst);
        p[i].end_time = current_time + p[i].burst;
        current_time += p[i].burst;
        i++;
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
}
// Shortest Job First
void solve_sjf(Process *orig, int count) {
    Process p[100];
    memcpy(p, orig, sizeof(Process) * count);
    qsort(p, count, sizeof(Process), compare_arrival);

    printf("ALG SJF\n");
    printf("GANTT");

    int current_time = 0;
    int completed = 0;
    
    while (completed < count) {
        int idx = -1;
        int min_burst = 1000000;
    
        for (int i = 0; i < count; i++) {
            if (!p[i].completed && p[i].arrival <= current_time) {
                if (p[i].burst < min_burst) {
                    min_burst = p[i].burst;
                    idx = i;
                } else if (p[i].burst == min_burst) {
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
            int next_arrival = 1000000;
            for(int i=0; i<count; i++) {
                if(!p[i].completed && p[i].arrival < next_arrival) 
                    next_arrival = p[i].arrival;
            }
            if (next_arrival < 1000000) {
                printf(" IDLE@%d-%d", current_time, next_arrival);
                current_time = next_arrival;
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
}

int main() {
    char line[256];
    Process procs[100];
    int count = 0;

    // Skip header
    if (!fgets(line, sizeof(line), stdin)) return 0;

    while (fgets(line, sizeof(line), stdin)) {
        char *pid_tok = strtok(line, ", \n");
        char *arr_tok = strtok(NULL, ", \n");
        char *bst_tok = strtok(NULL, ", \n");

        if (!pid_tok || !arr_tok || !bst_tok) print_error(E_INPUT, "malformed csv");
        
        strncpy(procs[count].pid, pid_tok, 31);
        procs[count].arrival = atoi(arr_tok);
        procs[count].burst = atoi(bst_tok);
        procs[count].completed = 0;

        if (procs[count].arrival < 0 || procs[count].burst <= 0) 
            print_error(E_RANGE, "arrival/burst range error");

        count++;
    }

    check_duplicates(procs, count);
    if (count == 0) return 0;

    solve_fcfs(procs, count);
    solve_sjf(procs, count);

    return 0;
}

