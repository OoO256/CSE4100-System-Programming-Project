#ifndef CSE4100_ASSEMBLER_H
#define CSE4100_ASSEMBLER_H

#define MAX_OBJ_PER_LINE 60
#define MAX_LINE_LENGTH 200
#define NO_REGISTRE -1
#define MAX_LINE_ASM 10000

enum assemble_error{
    twice_declared = 0,
    undefined_mnemonic,
    undefined_symbol,
    undefined_label,
    memory_boundary,
    not_asm_file,
    START_not_exist
};
// enums for errors

struct format1 {
    union {
        struct {
            unsigned int opcode : 8;
        };
        unsigned int all;
    };
};
// bit field for format 1

struct format2 {
    union {
        struct {
            unsigned int r2 : 4;
            unsigned int r1 : 4;
            unsigned int opcode : 8;
        };
        unsigned int all;
    };
};
// bit field for format 2

struct format3 {
    union {
        struct {
            unsigned int disp : 12;
            unsigned int e : 1;
            unsigned int p : 1;
            unsigned int b : 1;
            unsigned int x : 1;
            unsigned int i : 1;
            unsigned int n : 1;
            unsigned int opcode : 6;
        };
        unsigned int all;
    };
};
// bit field for format 3

struct format4 {
    union {    // 익명 공용체
        struct {    // 익명 구조체
            unsigned int address : 20;
            unsigned int e : 1;
            unsigned int p : 1;
            unsigned int b : 1;
            unsigned int x : 1;
            unsigned int i : 1;
            unsigned int n : 1;
            unsigned int opcode : 6;
        };
        unsigned int all;
    };
};
// bit field for format 4

struct list_line{
    int isCommnet;
    int obj_size;

    char* str;

    unsigned int loc;
    char label[10];
    char mnemonic[10];
    char operand[10];
    unsigned int obj;
    char data[30+1];
};
// to store information of each line of list file

int type(char* filename);
int assemble(char* filename_asm);

#endif //CSE4100_ASSEMBLER_H
