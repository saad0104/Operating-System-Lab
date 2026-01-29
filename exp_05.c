// ##########  Process Spwaner ########

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define E_USAGE "E_USAGE"
#define E_FORK "E_FORK"
#define E_EXEC "E_EXEC"
#define E_WAIT "E_WAIT"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    char *cmd = NULL;
    char *args_str = NULL;
    int repeat = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--cmd") == 0) {
            if (i + 1 < argc) cmd = argv[++i];
        } else if (strcmp(argv[i], "--args") == 0) {
            if (i + 1 < argc) args_str = argv[++i];
        } else if (strcmp(argv[i], "--repeat") == 0) {
            if (i + 1 < argc) repeat = atoi(argv[++i]);
        }
    }

    if (!cmd) print_error(E_USAGE, "missing --cmd");
    if (repeat < 1) print_error(E_USAGE, "repeat must be >= 1");

    
    char *exec_args[64];
    exec_args[0] = cmd;
    int arg_idx = 1;

    if (args_str) {
        char *token = strtok(args_str, ",");
        while (token != NULL && arg_idx < 63) {
            exec_args[arg_idx++] = token;
            token = strtok(NULL, ",");
        }
    }
    exec_args[arg_idx] = NULL;

    for (int k = 1; k <= repeat; k++) {
        pid_t pid = fork();

        if (pid < 0) {
            print_error(E_FORK, "fork failed");
        } else if (pid == 0) {
            // Child process
            execvp(cmd, exec_args);
            // If exec returns, it failed
            // exit code (127) to indicate exec failure internally
            fprintf(stderr, "ERROR: E_EXEC: cannot exec program\n");
            exit(127); 
        } else {
            // Parent process
            printf("CHILD %d PID %d START\n", k, pid);
            
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                print_error(E_WAIT, "wait failed");
            }

            if (WIFEXITED(status)) {
                int code = WEXITSTATUS(status);
                if (code == 127) {
                    // Map child's exec failure to our main error
                    // But specs say print error line and exit non-zero immediately
                     print_error(E_EXEC, "cannot exec program");
                }
                printf("CHILD %d PID %d EXIT %d\n", k, pid, code);
            } else if (WIFSIGNALED(status)) {
                printf("CHILD %d PID %d SIG %d\n", k, pid, WTERMSIG(status));
            }
        }
    }

    printf("OK: COMPLETED %d\n", repeat);
    return 0;
}