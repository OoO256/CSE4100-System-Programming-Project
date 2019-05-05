#include <stdint.h>
#include "run.h"
#include "memory.h"
#include "linking_loader.h"
#include "20171667.h"
#include "opcode.h"
#include "assembler.h"

#define r1 *addr_to_reg(extract_uint8(inst.b1, 0, 4))
#define r2 *addr_to_reg(extract_uint8(inst.b1, 4, 8))
#define WORD0 (word){.all = 0}
#define INPUT_DATA 1

const int POS_N = 6;
const int POS_I = 7;
const int POS_X = 0;
const int POS_B = 1;
const int POS_P = 2;
const int POS_E = 3;

typedef struct word{
    union{
        struct{
            uint8_t b3;
            uint8_t b2;
            uint8_t b1;
            uint8_t b0;
        };
        uint32_t all;
    };
}word;

struct Float{
    u_int64_t frac : 36;
    u_int64_t exp : 11;
    u_int64_t sign : 1;
};

static uint8_t set_uint8(uint8_t src, int start, int end){

    for(int i = start; i < end; i++){
        src |= (1 << i);
    }
    return src;
}

static word set_word(word src, int start, int end){

    for(int i = start; i < end; i++){
        if(i < 8){
            src.b0 = set_uint8(src.b0, i, i+1);
        }
        else if (i < 16){
            src.b1 = set_uint8(src.b1, i - 8, i+1 - 8);
        }
        else if (i < 24){
            src.b2 = set_uint8(src.b2, i - 16, i+1 - 16);
        }
        else if (i < 32){
            src.b3 = set_uint8(src.b3, i - 24, i+1 - 24);
        }
    }
    return src;
}

static int check_uint8(uint8_t src, int pos){
    return (src & (1 << pos)) != 0;
}

static int check_word(word src, int pos){
    if(pos < 8){
        return check_uint8(src.b0, pos);
    }
    else if (pos < 16){
        return check_uint8(src.b1, pos - 8);
    }
    else if (pos < 24){
        return check_uint8(src.b2, pos - 16);
    }
    else if (pos < 32){
        return check_uint8(src.b3, pos - 24);
    }
}

static uint8_t extract_uint8(uint8_t src, int start, int end){
    src &= set_uint8(0, start, end);
    src >>= 8-end;
    return src;
}

static word extract_word(word src, int start, int end){

    src.all &= set_word( (word){.all= 0}, start, end).all;
    src.all >>= 32 - end;
    return src;
}

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

static u_int32_t deref(u_int32_t src){
    src &= set_word(WORD0, 0, 8).all;

    return (u_int32_t)memory[src] * (1 << 16)
        + (u_int32_t)memory[src+1] * (1 << 8)
        + (u_int32_t)memory[src+2];
}

static void store(uint32_t TA, uint32_t val){
    word w = {.all = val};
    memory[TA] = w.b1;
    memory[TA] = w.b2;
    memory[TA] = w.b3;
}


int run(){
    PC = progaddr + execute_addr;

    while (progaddr <= PC && PC < progaddr + total_length){
        int format;

        word inst = {.b0 = memory[PC], .b1 = memory[PC+1], .b2 = memory[PC+2], .b3 = memory[PC+3]};
        word disp12 = {.all = inst.all & set_word((word){.all = 0}, 12, 24).all};
        word disp20 = {.all = inst.all & set_word((word){.all = 0}, 12, 32).all};
        uint8_t opcode = extract_uint8(inst.b0, 0, 6) << 2;

        format = get_format_from_opcode(opcode);


        int n;
        int bit;
        uint32_t TA;
        uint32_t TV;

        switch (format) {
            case 1:
                switch (opcode) {
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
                        //  address of channel program
                        //  is given by (S)
                    case 0xF8:
                        // Test I/O channel number (A)
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch (opcode) {
                    case 0x90:
                        //ADDR r1,r2      2       90    r2 <-- (r2) + (r1)
                        r2 = r2 + r1;
                        break;
                    case 0xB4:
                        // CLEAR r1        2       B4    r1 <-- 0
                        r2 = 0;
                        break;
                    case 0xA0:
                        // COMPR r1,r2     2       A0    (r1) : (r2)
                        SW = r1 - r2;
                        break;
                    case 0x9C:
                        // DIVR r1,r2      2       9C    (r2) <-- (r2) / (r1)
                        r2 = r2 / r1;
                        break;
                    case 0x98:
                        // MULR r1,r2      2       98    r2 <-- (r2) * (r1)
                        r2 = r2 * r1;
                        break;
                    case 0xAC:
                        // RMO r1,r2       2       AC    r2 <-- (r1)
                        r2 = r1;
                        break;
                    case 0xA4:
                        // SHIFTL r1,n     2       A4    r1 <-- (r1); left circular            X
                        //                                shift n bits. {In assembled
                        // instruction, r2=n-1}
                        bit = ((1 << 23) & r1) != 0;
                        n = r2 + 1;
                        r1 <<= n;
                        r1 |= bit;
                        break;
                    case 0x94:
                        // SUBR r1,r2      2       94    r2 <-- (r2) - (r1)
                        r2 = r2 - r1;
                        break;
                    case 0xB0:
                        // SVC n           2       B0    Generate SVC interrupt. {In           X
                        //                                assembled instruction, r1=n}
                        //
                        break;
                    case 0xB8:
                        // X <-- (X) + 1; (X) : (r1)
                        X++;
                        SW = X - r1;
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                if (check_uint8(inst.b1, POS_E)) {
                    TA = extract_word(inst, 12, 32).all;
                    format = 4;
                } else if (!check_uint8(inst.b0, POS_N) && !check_uint8(inst.b0, POS_I)) {
                    // SIC opcode
                    TA = extract_word(inst, 8 + 1, 24).all;
                } else if (check_uint8(inst.b1, POS_B) && !check_uint8(inst.b1, POS_P)) {
                    // base relative
                    TA = deref(B) + disp12.all;
                } else if (!check_uint8(inst.b1, POS_B) && check_uint8(inst.b1, POS_P)) {
                    // pc relative
                    TA = deref(PC) + disp12.all;
                    TA &= set_word(WORD0, 20, 32).all;
                } else {
                    printf("not supported addressing!\n");
                }

                if (check_uint8(inst.b1, POS_X)) {
                    TA += X;
                }

                printf("target addr is %x\n", TA);

                switch (extract_uint8(inst.b0, 6, 8)) {
                    case 0b00000001:
                        TV = TA;
                        break;
                    case 0b00000000:
                    case 0b00000011:
                        TV = deref(TA);
                        break;
                    case 0b00000010:
                        TA = deref(TA);
                        TV = deref(TA);
                        break;
                    default:
                        // no case
                        break;
                }

                printf("Target value is %x\n", TV);

                switch (extract_uint8(inst.b0, 0, 6)) {
                    case 0x18:
                        // ADD m          3/4      18    A <-- (A) + (m..m+2)
                        A = deref(A) + TV;
                        break;
                    case 0x58:
                        // ADDF m         3/4      58    F <-- (F) + (m..m+5)
                        // TODO
                        break;
                    case 0x40:
                        // AND m          3/4      40    A <-- (A) & (m..m+2
                        A = deref(A) & TV;
                        break;
                    case 0x28:
                        // COMP m         3/4      28    A : (m..m+2)
                        SW = A - TV;
                        break;
                    case 0x88:
                        // COMPF m        3/4      88    F : (m..m+5)
                        // TODO
                        break;
                    case 0x24:
                        //DIV m          3/4      24    A : (A) / (m..m+2)
                        A = deref(A) / TV;
                        break;
                    case 0x64:
                        // DIVF m         3/4      64    F : (F) / (m..m+5)
                        // TODO
                        break;
                    case 0x3C:
                        //J m            3/4      3C    PC <-- m
                        PC = TA;
                        break;
                    case 0x30:
                        //JEQ m          3/4      30    PC <-- m if CC set to =
                        if (SW == 0) {
                            PC = TA;
                        }
                        break;
                    case 0x34:
                        //JGT m          3/4      34    PC <-- m if CC set to >
                        if (SW > 0) {
                            PC = TA;
                        }
                        break;
                    case 0x38:
                        //JLT m          3/4      38    PC <-- m if CC set to <
                        if (SW < 0) {
                            PC = TA;
                        }
                        break;
                    case 0x48:
                        //JSUB m         3/4      48    L <-- (PC); PC <-- m
                        L = deref(PC);
                        PC = TA;
                        break;
                    case 0x00:
                        //LDA m          3/4      00    A <-- (m..m+2)
                        A = TV;
                        break;
                    case 0x68:
                        //LDB m          3/4      68    B <-- (m..m+2)                        X
                        B = TV;
                        break;
                    case 0x50:
                        //LDCH m         3/4      50    A [rightmost byte] <-- (m)
                        TV &= set_word(WORD0, 24, 32).all;
                        A &= set_word(WORD0, 0, 24).all;
                        A |= TV;
                        break;
                    case 0x70:
                        //LDF m          3/4      70    F <-- (m..m+5)                        X F
                        // TODO
                        break;
                    case 0x08:
                        //LDL m          3/4      08    L <-- (m..m+2)
                        L = TV;
                        break;
                    case 0x6C:
                        //LDS m          3/4      6C    S <-- (m..m+2)                        X
                        S = TV;
                        break;
                    case 0x74:
                        //LDT m          3/4      74    T <-- (m..m+2)                        X
                        T = TV;
                        break;
                    case 0x04:
                        //LDX m          3/4      04    X <-- (m..m+2)
                        X = TV;
                        break;
                    case 0xD0:
                        //LPS m          3/4      D0    Load processor status from
                        //                                information beginning at
                        //                                address m (see Section
                        //                                6.2.1)
                        // TODO
                        break;
                    case 0x20:
                        //MUL m          3/4      20    A <-- (A) * (m..m+2)
                        A = deref(A) * TV;
                        A &= 0b00000000111111111111111111111111;
                        break;
                    case 0x60:
                        //MULF m         3/4      60    F <-- (F) * (m..m+5)                  X F
                        // TODO
                        break;
                    case 0x44:
                        //OR m           3/4      44    A <-- (A) | (m..m+2)
                        A = deref(A) | TV;
                        break;
                    case 0xD8:
                        //RD m           3/4      D8    A [rightmost byte] <-- data         P
                        //                                from device specified by (m)
                        A &= set_word(WORD0, 0, 24).all;
                        A |= INPUT_DATA;
                        break;
                    case 0x4C:
                        //RSUB           3/4      4C    PC <-- (L)
                        PC = deref(L);
                        break;
                    case 0xEC:
                        //SSK m          3/4      EC    Protection key for address m        P X
                        //                                <-- (A) (see Section 6.2.4)
                        // TODO
                        break;
                    case 0x0C:
                        //STA m          3/4      0C    m..m+2 <-- (A)
                        store(TA, A);
                        break;
                    case 0x78:
                        //STB m          3/4      78    m..m+2 <-- (B)                        X
                        store(TA, B);
                        break;
                    case 0x54:
                        //STCH m         3/4      54    m <-- (A) [rightmost byte]
                        memory[TA]
                                = (uint8_t) (deref(A));
                        break;
                    case 0x80:
                        //STF m          3/4      80    m..m+5 <-- (F)                        X
                        // TODO
                        break;
                    case 0xD4:
                        //STI m          3/4      D4    Interval timer value <--            P X
                        //                                (m..m+2) (see Section
                        //                                6.2.1)
                        // we dont support it
                        break;
                    case 0x14:
                        //STL m          3/4      14    m..m+2 <-- (L)
                        store(TA, deref(L));
                        break;
                    case 0x7C:
                        //STS m          3/4      7C    m..m+2 <-- (S)                        X
                        store(TA, deref(S));
                        break;
                    case 0xE8:
                        //STSW m         3/4      E8    m..m+2 <-- (SW)                     P
                        store(TA, deref(SW));
                        break;
                    case 0x84:
                        //STT m          3/4      84    m..m+2 <-- (T)                        X
                        store(TA, deref(T));
                        break;
                    case 0x10:
                        //STX m          3/4      10    m..m+2 <-- (X)
                        store(TA, deref(X));
                        break;
                    case 0x1C:
                        //SUB m          3/4      1C    A <-- (A) - (m..m+2)
                        A = deref(A) - TV;
                        break;
                    case 0x5C:
                        //SUBF m         3/4      5C    F <-- (F) - (m..m+5)                  X F
                        // TODO
                        break;
                    case 0xE0:
                        //TD m           3/4      E0    Test device specified by (m)
                        // IO
                        break;
                    case 0x2C:
                        //TIX m          3/4      2C    X <-- (X) + 1; (X) : (m..m+2)             C
                        X = deref(X) + 1;
                        SW = deref(X) - TV;
                        break;
                    case 0xDC:
                        //WD m           3/4      DC    Device specified by (m) <-- (A)     P
                        // IO
                        break;
                    default:
                        printf("not supported opcode!\n");
                        break;

                }
            default:
                break;
        }
        PC += format;
    }
}