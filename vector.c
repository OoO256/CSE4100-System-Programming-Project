#include <stdio.h>

#include "vector.h"

static int emplace_back (struct vector* this, char name[7], unsigned int addr, int is_control_section, int length){

    this->size++;
    this->data = realloc(this->data, this->size * sizeof(external_symbol));


    strncpy(this->data[this->size - 1].name, name, 6);
    this->data[this->size - 1].addr = addr;
    this->data[this->size - 1].is_control_section = is_control_section;
    this->data[this->size - 1].length = length;
    return 0;
}

static external_symbol* get (struct vector* this, int idx){

    if(idx >= this->size || idx < 1)
        return NULL;

    return &(this->data[idx-1]);
}

static void print(struct vector* this){
    printf("control\t symbol\t address\t length\n");
    printf("section\t name\n");

    for (int i=0;i<40;i++)
        printf("-");
    printf("\n");


    unsigned int total_length = 0;

    for (int i = 0; i < this->size; ++i) {
        if(this->data[i].is_control_section){
            printf("%6s\t       \t %04X\t %04X\n", this->data[i].name, this->data[i].addr, this->data[i].length);
            total_length += this->data[i].length;
        }
        else{
            printf("      \t %6s\t %04X\t %04X\n", this->data[i].name, this->data[i].addr, this->data[i].length);
        }
    }

    for (int i=0;i<40;i++)
        printf("-");
    printf("\n");
    printf("total length : %X\n", total_length);
}

static struct vector new(){

    return (struct vector){
            .size = 0,
            .data = malloc(0),
            .emplace_back = &emplace_back,
            .get = &get,
            .print = &print
    };
}

const struct vector_class vector = {.new = &new};
