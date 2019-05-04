#ifndef PROJECT_HASHTABLE_H
#define PROJECT_HASHTABLE_H

#include <string.h>
#include <stdlib.h>

#define NOT_FOUND NULL

typedef struct node_external_symbol{
    char name[7];
    unsigned int addr;
    int is_control_section;
    int length;
}external_symbol;


struct vector {

    int size;
    external_symbol* data;

    int (*emplace_back)(struct vector* this, char name[7], unsigned int addr, int is_control_section, int length);
    external_symbol* (*get) (struct vector* this, int idx);
    void (*print)(struct vector* this);
};

extern const struct vector_class{
    struct vector (*new)(void);
} vector;




#endif //PROJECT_HASHTABLE_H
