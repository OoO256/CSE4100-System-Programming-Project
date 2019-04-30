#include "hashtable.h"

#define SAME(x,y) (!strcmp((x),(y)))

external_symbol* make_external_symbol(char name[6], unsigned int addr, char control_section[6]){
    external_symbol* mine = (external_symbol*)malloc(sizeof(external_symbol));
    strncpy(mine->name, name, 6);
    mine->addr = addr;
    strncpy(mine->control_section, control_section, 6);
    mine->next = NULL;

    return mine;
}

static int push(struct hashtable* this, external_symbol* sym){

    int hash_value = this->hash(this, sym->name);

    if(this->find(this, sym->name) != NOT_FOUND)
        return -1;

    external_symbol* curr = this->table[hash_value];
    while (curr != NULL){
        curr = curr->next;
    }
    curr = sym;
    return 0;
}

static int hash(struct hashtable* this, char name[6]){

    int hash = 0;
    for (int i = 0; i < 6; i++){
        hash += (int)name[i];
    }

    return hash % this->size;
}

external_symbol* find(struct hashtable* this, char name[6]){
    int hash_value = hash(this, name);

    external_symbol* curr = this->table[hash_value];

    while (curr != NULL){
        if(SAME(curr->name, name)){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    return NOT_FOUND;
}

static struct hashtable new(int size){

    return (struct hashtable){
            .size = size,
            .table = calloc(size, sizeof(external_symbol)),
            .push = &push,
            .hash = &hash,
            .find = &find
    };
}

const struct hashtable_class hashtable = {.new = &new};
