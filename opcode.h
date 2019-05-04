#pragma once

#define LENGTH_HASHTABLE 20

#include "20171667.h"

typedef struct _node_instruction {
    int opcode;
    char mnemonic[MAX_COMMAND_LENGTH];
    char format[MAX_COMMAND_LENGTH];
    struct _node_instruction* next;
} node_instruction;
// to store information of instruction

node_instruction** hashTable;

void pushToHash(node_instruction* inst);

void initHashTable();

void freeHashTable();

void opcodelist();

node_instruction* getInst(char* mnemonic);

int opcode(char* mnemonic);

char* removeSpaces(char* str);

int get_format_from_opcode(int opcode);