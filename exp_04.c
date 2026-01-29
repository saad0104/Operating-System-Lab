// ########  Txt pattern Search ############

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_USAGE "E_USAGE"
#define E_EMPTY_PATTERN "E_EMPTY_PATTERN"
#define E_OPEN "E_OPEN"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    char *pattern = NULL;
    char *files_str = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--pattern") == 0) {
            if (i + 1 < argc) pattern = argv[++i];
        } else if (strcmp(argv[i], "--files") == 0) {
            if (i + 1 < argc) files_str = argv[++i];
        }
    }

    if (!pattern) print_error(E_USAGE, "missing --pattern");
    if (strlen(pattern) == 0) print_error(E_EMPTY_PATTERN, "pattern must be non-empty");
    if (!files_str) print_error(E_USAGE, "missing --files");

    char *file_list[32];
    int file_count = 0;
    
    // Split files by comma
    char *token = strtok(files_str, ",");
    while (token != NULL) {
        file_list[file_count++] = token;
        token = strtok(NULL, ",");
    }

    int total_matches = 0;

    for (int i = 0; i < file_count; i++) {
        FILE *fp = fopen(file_list[i], "r");
        if (!fp) print_error(E_OPEN, "cannot open file");

        char line[4096];
        int line_no = 1;

        while (fgets(line, sizeof(line), fp)) {
            // Remove newline
            char *newline = strchr(line, '\n');
            if (newline) *newline = '\0';

            if (strstr(line, pattern)) {
                printf("MATCH %s:%d:%s\n", file_list[i], line_no, line);
                total_matches++;
            }
            line_no++;
        }
        fclose(fp);
    }

    printf("OK: MATCHES %d FILES %d\n", total_matches, file_count);
    return 0;
}