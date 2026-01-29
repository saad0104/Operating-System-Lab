// ########## Pipe Based Filter Chain ###########
// pipechain

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define E_USAGE "E_USAGE"
#define E_STAGE "E_STAGE"

typedef struct {
    char *cmd;
    char *args;
    pid_t pid;
    char *name;
} Stage;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

void parse_and_exec(char *cmd, char *args_str) {
    char *exec_args[64];
    exec_args[0] = cmd;
    int idx = 1;
    if (args_str) {
        char *token = strtok(args_str, ",");
        while (token && idx < 63) {
            exec_args[idx++] = token;
            token = strtok(NULL, ",");
        }
    }
    exec_args[idx] = NULL;
    execvp(cmd, exec_args);
    exit(127);
}

int main(int argc, char *argv[]) {
    Stage stages[3] = { {NULL, NULL, 0, "producer"}, {NULL, NULL, 0, "filter"}, {NULL, NULL, 0, "consumer"} };

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--producer") == 0) stages[0].cmd = argv[++i];
        else if (strcmp(argv[i], "--producer-args") == 0) stages[0].args = argv[++i];
        else if (strcmp(argv[i], "--filter") == 0) stages[1].cmd = argv[++i];
        else if (strcmp(argv[i], "--filter-args") == 0) stages[1].args = argv[++i];
        else if (strcmp(argv[i], "--consumer") == 0) stages[2].cmd = argv[++i];
        else if (strcmp(argv[i], "--consumer-args") == 0) stages[2].args = argv[++i];
    }

    if (!stages[0].cmd || !stages[1].cmd || !stages[2].cmd) print_error(E_USAGE, "missing stage cmd");

    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    int dev_null = open("/dev/null", O_WRONLY);

    //  Producer
    if ((stages[0].pid = fork()) == 0) {
        dup2(p1[1], STDOUT_FILENO); 
        dup2(dev_null, STDERR_FILENO); 
        close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]); close(dev_null);
        parse_and_exec(stages[0].cmd, stages[0].args);
    }

    // Filter
    if ((stages[1].pid = fork()) == 0) {
        dup2(p1[0], STDIN_FILENO);  
        dup2(p2[1], STDOUT_FILENO); 
        dup2(dev_null, STDERR_FILENO);
        close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]); close(dev_null);
        parse_and_exec(stages[1].cmd, stages[1].args);
    }

    // Consumer
    if ((stages[2].pid = fork()) == 0) {
        dup2(p2[0], STDIN_FILENO);  
        dup2(dev_null, STDOUT_FILENO); 
        dup2(dev_null, STDERR_FILENO);
        close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]); close(dev_null);
        parse_and_exec(stages[2].cmd, stages[2].args);
    }

    // Parent closes all pipes
    close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]); close(dev_null);

    // Wait and Check
    int failure = 0;
    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(stages[i].pid, &status, 0);
        
        if (!failure) { 
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "ERROR: E_STAGE: stage %s exit %d\n", stages[i].name, WEXITSTATUS(status));
                failure = 1;
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "ERROR: E_STAGE: stage %s sig %d\n", stages[i].name, WTERMSIG(status));
                failure = 1;
            }
        }
    }

    if (failure) exit(1);
    printf("OK: PIPELINE SUCCESS\n");
    return 0;
}