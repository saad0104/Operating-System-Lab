// ############ Shared Memory Counter ########## 
// shmcounter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

#define E_USAGE "E_USAGE"
#define E_RANGE "E_RANGE"
#define E_SHM "E_SHM"
#define E_MMAP "E_MMAP"
#define E_SEM "E_SEM"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int procs = 0, iters = 0;
    char *name = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--procs") == 0) procs = atoi(argv[++i]);
        else if (strcmp(argv[i], "--iters") == 0) iters = atoi(argv[++i]);
        else if (strcmp(argv[i], "--name") == 0) name = argv[++i];
    }

    if (procs < 2 || procs > 16) print_error(E_RANGE, "procs must be in 2..16");
    if (iters < 1 || iters > 100000) print_error(E_RANGE, "iters must be in 1..100000");
    if (!name) print_error(E_USAGE, "missing name");
    
    // Validate name
    for (int i = 0; name[i]; i++) {
        if (!isalnum(name[i])) print_error(E_USAGE, "name must be alphanumeric");
    }

    // Construct names
    char shm_name[64], sem_name[64];
    snprintf(shm_name, 64, "/shm_%s", name);
    snprintf(sem_name, 64, "/sem_%s", name);

    // Create Shared Memory
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) print_error(E_SHM, "shm_open failed");
    if (ftruncate(shm_fd, sizeof(int64_t)) == -1) print_error(E_SHM, "ftruncate failed");

    int64_t *counter = mmap(NULL, sizeof(int64_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) print_error(E_MMAP, "mmap failed");
    
    *counter = 0;

    // Create Semaphore
    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) print_error(E_SEM, "sem_open failed");

    // Fork and Increment
    for (int i = 0; i < procs; i++) {
        if (fork() == 0) {
            for (int j = 0; j < iters; j++) {
                sem_wait(sem);
                (*counter)++;
                sem_post(sem);
            }
            exit(0);
        }
    }

    // Wait
    for (int i = 0; i < procs; i++) wait(NULL);

    // Output
    printf("OK: FINAL %ld\n", (long)*counter);

    // Clean
    munmap(counter, sizeof(int64_t));
    shm_unlink(shm_name);
    sem_close(sem);
    sem_unlink(sem_name);

    return 0;
}