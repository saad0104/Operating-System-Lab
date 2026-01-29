// ##########  Timeout Supervisor ########

#define _POSIX_C_SOURCE 200809L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

// error messages
#define E_USAGE "E_USAGE"
#define E_RANGE "E_RANGE"
#define E_FORK "E_FORK"
#define E_EXEC "E_EXEC"
#define E_WAIT "E_WAIT"
#define E_SIGNAL "E_SIGNAL"

volatile sig_atomic_t timed_out = 0;
pid_t child_pid = 0;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

void handle_alarm(int sig) {
    timed_out = 1;
   // kill the child
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);
    }
}

int main(int argc, char *argv[]) {
    int seconds = 0;
    char *cmd = NULL;
    char *args_str = NULL;

    // Parsing command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seconds") == 0) {
            if (i + 1 < argc) seconds = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--cmd") == 0) {
            if (i + 1 < argc) cmd = argv[++i];
        } else if (strcmp(argv[i], "--args") == 0) {
            if (i + 1 < argc) args_str = argv[++i];
        }
    }

    // input validation
    if (seconds < 1 || seconds > 60) print_error(E_RANGE, "seconds must be in 1..60");
    if (!cmd) print_error(E_USAGE, "missing --cmd");

    // Prepare arguments for execvp
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

    // Configure the Signal Handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);

    child_pid = fork();
 
    if (child_pid < 0) {
        print_error(E_FORK, "fork failed");
    } 
    else if (child_pid == 0) {
        // ## Child Process ##
        execvp(cmd, exec_args);
        exit(127); 
    } 
    else {
       // starts the timer
        alarm(seconds);

        int status;
        // block until child exists
        if (waitpid(child_pid, &status, 0) < 0) {
            print_error(E_WAIT, "wait failed");
        }

        alarm(0); // Disable the timer  if child finishes

        
        if (timed_out) {
            printf("OK: TIMEOUT KILLED\n");
        } else if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code == 127) print_error(E_EXEC, "cannot exec program");
            printf("OK: EXIT %d\n", code);
        } else if (WIFSIGNALED(status)) {
            printf("OK: SIG %d\n", WTERMSIG(status));
        }
    }

    return 0;
}