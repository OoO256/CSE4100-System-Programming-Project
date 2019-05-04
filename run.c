#include <stdint.h>
#include "run.h"
#include "memory.h"
#include "linking_loader.h"
#include "20171667.h"
#include "opcode.h"
#include "assembler.h"

const uint8_t N = 0b00000010;
const uint8_t I = 0b00000001;
const uint8_t X = 0b10000000;
const uint8_t B = 0b01000000;
const uint8_t P = 0b00100000;
const uint8_t E = 0b00010000;

int flag_on(uint8_t byte, uint8_t reg){
    return (byte | reg) != 0;
}


int run(){
    int A, X, L, PC = progaddr + execute_addr, B, S, T;


    while (progaddr <= PC && PC < progaddr + total_length){
        int format;

        uint8_t first_byte = memory[PC];
        uint8_t second_byte = memory[PC+1];


        if(flag_on(second_byte, E)){
            format = 4;
        }
        else{
            format = get_format_from_opcode(first_byte | 0b11111100);
        }

        switch (format){
            case 1:

        }


    }
}