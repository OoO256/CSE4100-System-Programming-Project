#include "symbol.h"

node_symbol** symbolTable = NULL;
int num_symbol = 0;
struct node_symbol* symbol_list = NULL;

void initSymbolTable(){
    if(symbolTable != NULL)
        freeSymbols();
        // if symbolTable used before, free them first


    symbolTable = (node_symbol**)calloc(LENGTH_SYMBOLTABLE, sizeof(node_symbol*));
    symbol_list = (node_symbol*)malloc(sizeof(node_symbol)*0);
}

void pushSymbol(char* symbol, int loc){
    // push symbols into symbolTable and symbolList

    int target_idx = hash(symbol);

    node_symbol* new_symbol = (node_symbol*)malloc(sizeof(node_symbol));
    new_symbol->next = NULL;
    strcpy(new_symbol->symbol, symbol);
    new_symbol->loc = loc;
    // make symbol structure

    if (symbolTable[target_idx] == NULL)
    {
        symbolTable[target_idx] = new_symbol;
    }
    else
    {
        node_symbol* curr = symbolTable[target_idx];
        while (curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_symbol;
    }
    // push it into symbol table


    num_symbol++;
    symbol_list = (node_symbol*)realloc(symbol_list, sizeof(node_symbol)*num_symbol);
    symbol_list[num_symbol- 1].loc = loc;
    strcpy(symbol_list[num_symbol- 1].symbol, symbol);
    // push it to symbol list
}

int getLoc(char* symbol){
    // return location of symbol
    // if cannot find, return NOT_FOUND(-1)

    int target_idx = hash(symbol);

    node_symbol* curr = symbolTable[target_idx];
    while (curr != NULL){
        if (!strcmp(curr->symbol, symbol)){
            return curr->loc;
        }
        curr = curr->next;
    }
    return NOT_FOUND;
}

int compareSymbol(const void* lhs, const void* rhs){
    // sort indexes by lexicographical order of symbol_list[*lhs].symbol
    return -1*strcmp(symbol_list[*(int*)lhs].symbol, symbol_list[*(int*)rhs].symbol);
}

int* sortedIndex(){
    // sort array of indexes by lexicographical order of symbol_list
    // to avoid swap, copy of symbols

    int* indexes = (int *)malloc(sizeof(int)* num_symbol);
    for (int i = 0; i < num_symbol; ++i) {
        indexes[i] = i;
    }

    qsort(indexes, num_symbol, sizeof(int), compareSymbol);

    return indexes;
}

void printSymbol(){
    // print symbols

    if(symbolTable == NULL)
        fprintf(stderr, "there is no symbol table yet\n");

    int* indexes = sortedIndex();

    for (int i = 0; i < num_symbol; ++i) {
        printf("\t%-6s\t%04X\n", symbol_list[indexes[i]].symbol, symbol_list[indexes[i]].loc);
    }
}

void freeSymbols(){
    //free symbols to avoid memory leak

    for (int i = 0; i < LENGTH_SYMBOLTABLE; i++)
    {
        DEBUG_PRINT(("i is %d\n", i));

        if (symbolTable[i] != NULL)
        {
            node_symbol* curr = symbolTable[i]->next;
            node_symbol* prev = symbolTable[i];

            while (curr != NULL) {
                DEBUG_PRINT(("%s get free!\n", prev->symbol));
                free(prev);

                prev = curr;
                curr = curr->next;
            }
            DEBUG_PRINT(("%s get free!\n", prev->symbol));
            free(prev);
        }
    }
    free(symbolTable);
    free(symbol_list);

    symbolTable = NULL;
    num_symbol = 0;
    symbol_list = NULL;
}
