#ifndef PROJECT_HASHTABLE_H
#define PROJECT_HASHTABLE_H

#include <string.h>
#include <stdlib.h>

#define NOT_FOUND NULL
#define HASHTABLE_SIZE 20

typedef struct node_external_symbol{
    char name[6];
    unsigned int addr;
    char control_section[6];

    struct node_external_symbol* next;
}external_symbol;


struct hashtable {

    int size;
    external_symbol** table;

    int (*push)(struct hashtable* this, external_symbol*);
    external_symbol* (*find)(struct hashtable* this, char name[6]);
    int (*hash) (struct hashtable* this, char name[6]);
};

extern const struct hashtable_class{
    struct hashtable (*new)(int data);
} hashtable;




#endif //PROJECT_HASHTABLE_H
