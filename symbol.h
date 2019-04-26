#ifndef CSE4100_SYMBOL_H
#define CSE4100_SYMBOL_H

#include "20171667.h"

#define NOT_FOUND (-1)
#define LENGTH_SYMBOLTABLE 20

typedef
struct node_symbol{
    int loc;
    char symbol[100];
    struct node_symbol* next;
}node_symbol;
// to store symbols

void initSymbolTable();
void pushSymbol(char* symbol, int loc);
int getLoc(char* symbol);
void printSymbol();
void freeSymbols();


#endif //CSE4100_SYMBOL_H
