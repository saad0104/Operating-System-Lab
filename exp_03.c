// #########  Directory Listing and Metadata ##########

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define E_USAGE "E_USAGE"
#define E_NOTDIR "E_NOTDIR"
#define E_OPEN_DIR "E_OPEN_DIR"
#define E_STAT "E_STAT"

typedef struct {
    char name[256];
    char type;
    long size;
} Entry;

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

// Comparison function for qsort
int compare_name(const void *a, const void *b) {
    return strcmp(((Entry*)a)->name, ((Entry*)b)->name);
}

int compare_size(const void *a, const void *b) {
    Entry *ea = (Entry*)a;
    Entry *eb = (Entry*)b;
    if (ea->size < eb->size) return -1;
    if (ea->size > eb->size) return 1;
    return strcmp(ea->name, eb->name);
}

int main(int argc, char *argv[]) {
    char *path = NULL;
    int sort_by_size = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--path") == 0) {
            if (i + 1 < argc) path = argv[++i];
        } else if (strcmp(argv[i], "--sort") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[++i], "size") == 0) sort_by_size = 1;
            }
        }
    }

    if (!path) print_error(E_USAGE, "missing --path");

    // path check
    struct stat path_stat;
    if (stat(path, &path_stat) < 0) {
        print_error(E_NOTDIR, "path not found or invalid"); 
    }
    if (!S_ISDIR(path_stat.st_mode)) {
        print_error(E_NOTDIR, "path is not a directory");
    }

    DIR *d = opendir(path);
    if (!d) print_error(E_OPEN_DIR, strerror(errno));

    Entry entries[1024];
    int count = 0;
    struct dirent *dir;

    int total = 0, files = 0, dirs = 0, links = 0, other = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;

        // Construct full path for lstat
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, dir->d_name);

        struct stat st;
        if (lstat(fullpath, &st) < 0) {
            print_error(E_STAT, strerror(errno));
        }

        char type = 'O';
        if (S_ISREG(st.st_mode)) { type = 'F'; files++; }
        else if (S_ISDIR(st.st_mode)) { type = 'D'; dirs++; }
        else if (S_ISLNK(st.st_mode)) { type = 'L'; links++; }
        else { other++; }

        strncpy(entries[count].name, dir->d_name, 255);
        entries[count].name[255] = '\0';
        entries[count].type = type;
        entries[count].size = st.st_size;
        count++;
        total++;
    }
    closedir(d);

    // Sort
    if (sort_by_size) {
        qsort(entries, count, sizeof(Entry), compare_size);
    } else {
        qsort(entries, count, sizeof(Entry), compare_name);
    }

    // Output
    for (int i = 0; i < count; i++) {
        printf("ENTRY %c %ld %s\n", entries[i].type, entries[i].size, entries[i].name);
    }
    printf("OK: TOTAL %d FILES %d DIRS %d LINKS %d OTHER %d\n", total, files, dirs, links, other);

    return 0;
}