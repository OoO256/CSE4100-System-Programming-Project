#include "20171667.h"
#include "memory.h"


int dump(int start, int end) {
    // do dump for [start, end] (inclusive)

    if (start < 0 || end >= MEMORY_SIZE){
        printf("out of memory boundery!\n");
        return UNSUCCESSFUL_RETURN;
    }

    if(start > end){
        printf("Wrong Memory Range!\n");
        return UNSUCCESSFUL_RETURN;
    }

    last_dump = end;
    // doing next dump, we start from end + 1

    int index_start = start - start % 16;
    int index_end = end - end % 16;

    for (int i = index_start; i <= index_end; i+=16)
    {
        printf("%05X ", i);
        for (int j = 0; j < 16; j++)
        {
            if (start <= i + j && i + j <= end)
            {
                // if [i+j] is in the right range [start, end], we print it
                printf("%02X ", memory[i + j]);
            }
            else
            {
                // else we prinf '   '
                printf("   ");
            }
        }
        printf("; ");

        for (int j = 0; j < 16; j++)
        {
            if (0x20 <= memory[i + j] && memory[i + j] <= 0x7E)
            {
                printf("%c", memory[i + j]);
                // if the value of the memory is in [0x20, 0x7E], we print it as ASCII character
            }
            else
            {
                printf(".");
                //else we print .
            }
        }
        printf("\n");
    }
    return SUCCESSFUL_RETURN;
}


int edit(int address, int value)
{
    // edit value of single byte in memory
    if (address < 0 || MEMORY_SIZE <= address){
        printf("Out of Memory Range!\n");
        return UNSUCCESSFUL_RETURN;
    }
    else if (value < 0x00 || 0xFF < value){
        printf("Value Error!\n");
        return UNSUCCESSFUL_RETURN;
    }
    else{
        memory[address] = value;
        return SUCCESSFUL_RETURN;
    }
}

int fill(int start, int end, int value)
{
    // fill [start, end] byte with value
    if (start < 0 || MEMORY_SIZE <= end){
        printf("Out of Memory Range!\n");
        return UNSUCCESSFUL_RETURN;
    }
    else if(start > end){
        printf("Wrong Memory Range!\n");
        return UNSUCCESSFUL_RETURN;
    }
    else if (value < 0x00 || 0xFF < value){
        // check value
        printf("Value Error!\n");
        return UNSUCCESSFUL_RETURN;
    }
    else{
        // fill [start, end]
        memset(memory + start, value, end - start + 1);
        return SUCCESSFUL_RETURN;
    }
}

void reset(){
    // set every byte with 00
    fill(0, MEMORY_SIZE -1, 0);
}

