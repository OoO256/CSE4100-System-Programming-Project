#include "20171667.h"
#include "breakpoints.h"

int bp[MEMORY_SIZE];

void clear_bp(){
    printf("[ok] clear all breakpoints\n");
    memset(bp, 0, MEMORY_SIZE);
}

void print_bp(){
    printf("breakpoint\n");
    printf("----------\n");
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        if(bp[i]){
            printf("%X\n", i);
        }
    }
}

void set_bp(int idx){
    bp[idx] = 1;
    printf("[ok] create breakpoint %X\n", idx);
}


int exist_bp(int idx){
    return bp[idx];
}

