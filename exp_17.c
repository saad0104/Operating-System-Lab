// Paging address translation
// pagetrans

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define E_INPUT "E_INPUT"
#define E_RANGE "E_RANGE"

void print_error(const char *code, const char *msg) {
    fprintf(stderr, "ERROR: %s: %s\n", code, msg);
    exit(1);
}

typedef struct {
    int vpn;
    int pfn;
    int valid;
} TLBEntry;

typedef struct {
    int vpn;
    int pfn;
    int valid;
} PageTableEntry;

int main(int argc, char *argv[]) {
    int pagesize = 0;
    int tlb_size = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--pagesize") == 0) pagesize = atoi(argv[++i]);
        else if (strcmp(argv[i], "--tlb") == 0) tlb_size = atoi(argv[++i]);
    }

    if (pagesize < 256 || pagesize > 65536 || (pagesize & (pagesize - 1)) != 0) 
        print_error(E_RANGE, "pagesize must be power of 2 in 256..65536");
    if (tlb_size < 0 || tlb_size > 64) print_error(E_RANGE, "tlb must be 0..64");

    int N;
    if (scanf("%d", &N) != 1) return 0;

    PageTableEntry *pt = malloc(sizeof(PageTableEntry) * N);
    for (int i = 0; i < N; i++) {
        if (scanf("%d %d %d", &pt[i].vpn, &pt[i].pfn, &pt[i].valid) != 3) 
            print_error(E_INPUT, "bad pt entry");
        if (pt[i].valid != 0 && pt[i].valid != 1) print_error(E_INPUT, "valid must be 0/1");
    }

    TLBEntry *tlb = NULL;
    if (tlb_size > 0) {
        tlb = malloc(sizeof(TLBEntry) * tlb_size);
        for(int i=0; i<tlb_size; i++) tlb[i].valid = 0; // mark invalid initially
    }

    int Q;
    if (scanf("%d", &Q) != 1) print_error(E_INPUT, "missing Q");

    unsigned int hits = 0, misses = 0;

    for (int k = 0; k < Q; k++) {
        unsigned int vaddr;
        if (scanf("%u", &vaddr) != 1) print_error(E_INPUT, "bad vaddr");

        unsigned int vpn = vaddr / pagesize;
        unsigned int offset = vaddr % pagesize;
        
        int pfn = -1;
        int fault = 0;
        int tlb_hit = 0;

        if (tlb_size > 0) {
            int idx = vpn % tlb_size;
            if (tlb[idx].valid && tlb[idx].vpn == vpn) {
                pfn = tlb[idx].pfn;
                tlb_hit = 1;
                hits++;
            } else {
                misses++;
            }
        }

        if (!tlb_hit) {
            int found_in_pt = 0;
            for (int i = 0; i < N; i++) {
                if (pt[i].vpn == vpn) {
                    if (pt[i].valid) {
                        pfn = pt[i].pfn;
                        found_in_pt = 1;
                        if (tlb_size > 0) {
                            int idx = vpn % tlb_size;
                            tlb[idx].vpn = vpn;
                            tlb[idx].pfn = pfn;
                            tlb[idx].valid = 1;
                        }
                    } else {
                        fault = 1;
                    }
                    
                    goto resolve;
                }
            }
            // Not in PT
            fault = 1; 
        }

    resolve:
        if (fault) {
            printf("OK: VA %u -> PAGEFAULT\n", vaddr);
        } else {
            unsigned int paddr = pfn * pagesize + offset;
            printf("OK: VA %u -> PA %u", vaddr, paddr);
            if (tlb_size > 0) {
                printf(" (TLB %s)\n", tlb_hit ? "HIT" : "MISS");
            } else {
                printf("\n");
            }
        }
    }

    if (tlb_size > 0) {
        printf("OK: TLB_HITS %u TLB_MISSES %u\n", hits, misses);
    }

    free(pt);
    if(tlb) free(tlb);
    return 0;
}