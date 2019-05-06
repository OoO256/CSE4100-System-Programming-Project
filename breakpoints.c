#include "20171667.h"
#include "breakpoints.h"

int bp[MEMORY_SIZE];
// bp 의 존재 여부 저장

void clear_bp(){
    // bp 모두 클리어
    printf("[ok] clear all breakpoints\n");
    memset(bp, 0, MEMORY_SIZE);
}

void print_bp(){
    // bp 출력
    printf("breakpoint\n");
    printf("----------\n");
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        if(bp[i]){
            printf("%X\n", i);
        }
    }
}

void set_bp(int idx){
    // bp 를 만든다
    bp[idx] = 1;
    printf("[ok] create breakpoint %X\n", idx);
}


int exist_bp(int idx){
    // bp의 존재 확인
    return bp[idx];
}

