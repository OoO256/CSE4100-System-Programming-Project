#include <stdint.h>
#include "run.h"
#include "memory.h"
#include "linking_loader.h"
#include "20171667.h"
#include "opcode.h"
#include "assembler.h"

struct Float{
    u_int64_t frac : 36;
    u_int64_t exp : 11;
    u_int64_t sign : 1;
};

const uint8_t POS_N = 0b00000010;
const uint8_t POS_I = 0b00000001;
const uint8_t POS_X = 0b10000000;
const uint8_t POS_B = 0b01000000;
const uint8_t POS_P = 0b00100000;
const uint8_t POS_E = 0b00010000;


static int32_t A, X, L, PC, B, S, T, SW;

int32_t* addr_to_reg(int addr){
    // return address of register
    // if ew cant find, return NO_REGISTER

    switch (addr) {
        case 0:
            return &A;
        case 1:
            return &X;
        case 2:
            return &L;
        case 3:
            return &B;
        case 4:
            return &S;
        case 5:
            return &T;
        case 8:
            return &PC;
        case 9:
            return &SW;
        default:
            return NULL;
    }
}

u_int32_t* r1(uint8_t byte){
    return addr_to_reg(byte >> 2);
}

u_int32_t* r2(uint8_t byte){
    return addr_to_reg(byte | 0b0011);
}

int flag_on(uint8_t byte, uint8_t pos){
    return (byte | pos) != 0;
}


int run(){
    PC = progaddr + execute_addr;

    while (progaddr <= PC && PC < progaddr + total_length){
        int format;

        uint8_t byte[4];
        byte[0] = memory[PC];
        byte[1] = memory[PC+1];


        if(flag_on(byte[1], POS_E)){
            format = 4;
        }
        else{
            format = get_format_from_opcode(byte[0] | 0b11111100);
        }

        switch (format){
            case 1:
                switch (byte[0] | 0b11111100){
                    case 0xC4:
                        // float
                    case 0xC0:
                        // float
                    case 0xF4:
                        // Halt I/O channel number (A)
                    case 0xC8:
                        // float
                    case 0xF0:
                        //  Start I/O channel number (A);       P X
                        //                                address of channel program
                        //                                is given by (S)
                    case 0xF8:
                        // Test I/O channel number (A)
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch (byte[0] | 0b11111100){
                    case 0x90:
                        //ADDR r1,r2      2       90    r2 <-- (r2) + (r1)
                        *r2(byte[1]) = *r2(byte[1]) + *r1(byte[1]);
                        break;
                    case 0xB4:
                        // CLEAR r1        2       B4    r1 <-- 0
                        *r2(byte[1]) = 0;
                        break;
                    case 0xA0:
                        // COMPR r1,r2     2       A0    (r1) : (r2)
                        SW = *r1(byte[1]) - *r2(byte[1]);
                        break;
                    case 0x9C:
                        // DIVR r1,r2      2       9C    (r2) <-- (r2) / (r1)
                        *r2(byte[1]) = *r2(byte[1]) / *r1(byte[1]);
                        break;
                    case 0x98:
                        // MULR r1,r2      2       98    r2 <-- (r2) * (r1)
                        *r2(byte[1]) = *r2(byte[1]) * *r1(byte[1]);
                        break;
                    case 0xAC:
                        // RMO r1,r2       2       AC    r2 <-- (r1)
                        *r2(byte[1]) = *r1(byte[1]);
                        break;
                    case 0xA4:
                        // SHIFTL r1,n     2       A4    r1 <-- (r1); left circular            X
                        //                                shift n bits. {In assembled
                        // instruction, r2=n-1}
                        break;
                    case 0x94:
                        // SUBR r1,r2      2       94    r2 <-- (r2) - (r1)
                        break;
                    case 0xB0:
                        // SVC n           2       B0    Generate SVC interrupt. {In           X
                        //                                assembled instruction, r1=n}
                        break;
                    case 0xB8:
                        X++;
                        SW = X - *r1(byte[1]);
                        break;
                    default:
                        break;
                }
                break;
            case 3:

        }


    }
}