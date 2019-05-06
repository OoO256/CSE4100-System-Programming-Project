#include <stdint.h>
#include "run.h"
#include "memory.h"
#include "linking_loader.h"
#include "20171667.h"
#include "opcode.h"
#include "assembler.h"
#include "breakpoints.h"

#define r1 *addr_to_reg(extract_uint8(inst.b1, 0, 4))
#define r2 *addr_to_reg(extract_uint8(inst.b1, 4, 8))
#define WORD0 (word){.all = 0}
//#define DEBUG

const int POS_N = 6;
const int POS_I = 7;
const int POS_X = 0;
const int POS_B = 1;
const int POS_P = 2;
const int POS_E = 3;
// 각 flag의 위치

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
// big endian 형식인 4바이트 정수

struct Float{
    u_int64_t frac : 36;
    u_int64_t exp : 11;
    u_int64_t sign : 1;
};
// float를 저장하는 구조체

static uint8_t set_uint8(uint8_t src, int start, int end){

    for(int i = start; i < end; i++){
        src |= (0b10000000 >> i);
    }
    return src;
}
// uint8의 [start, end)를 1로 set

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
// word의 [start, end)를 1로 set

static int check_uint8(uint8_t src, int pos){
    return (src & (0b10000000 >> pos)) != 0;
}
// uint8의 pos가 1인지 0인지 return

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
// word의 pos가 1인지 0인지 return

static uint8_t extract_uint8(uint8_t src, int start, int end){
    src &= set_uint8(0, start, end);
    src >>= 8-end;
    return src;
}
// uint8에서 [start, end)범위의 비트를 추출

static word extract_word(word src, int start, int end){

    src.all &= set_word( (word){.all= 0}, start, end).all;
    src.all >>= 32 - end;
    return src;
}
// word에서 [start, end)범위의 비트를 추출

int32_t A, X, L, PC, B, S, T, SW;
// 레지스터 변수
int from_bp = 0;
// breakpoint로 부터 실행인지, 첫 실행인지 저장

int32_t* addr_to_reg(int addr){
    // return pointer of register of given addr

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

static void print_reg(){
    // print information of register
    printf("A : %06X  ", A);
    printf("X : %06X  \n", X);
    printf("L : %06X  ", L);
    printf("PC: %06X  \n", PC);
    printf("B : %06X  ", B);
    printf("S : %06X  \n", S);
    printf("T : %06X  \n", T);
}

static uint32_t deref(uint32_t src){
    // derefernce src and fetch 3 byte
    word w = {.all = src};
    w.b0 = 0;

    return (uint32_t)memory[w.all] * (1 << 16)
        + (uint32_t)memory[w.all+1] * (1 << 8)
        + (uint32_t)memory[w.all+2];
}

static void store(uint32_t TA, uint32_t val){
    // store val into TA(target addr)
    word w = {.all = val};
    memory[TA] = w.b1;
    memory[TA+1] = w.b2;
    memory[TA+2] = w.b3;
}


int run(){
    const int is_input_test_device_ready = 1;
    // 디바이스가 준비되었는지 저장

    uint8_t device_input[] = {'I', 'N', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    int cnt_device_input = 0;
    // 디바이스의 입력을 저장

    if(from_bp == 0) {
        // if this is first run() call
        PC = progaddr + execute_addr;
        L = progaddr + total_length;
    }

    while (1){
        int format;
        // 현재 inst의 foramt을 저장

        word inst = {.b0 = memory[PC], .b1 = memory[PC+1], .b2 = memory[PC+2], .b3 = memory[PC+3]};
        // fetch (m .. m3) to parse
        word disp12 = {.all = extract_word(inst, 12, 24).all};
        word disp20 = {.all = extract_word(inst, 12, 32).all};
        uint8_t opcode = extract_uint8(inst.b0, 0, 6) << 2;
        format = get_format_from_opcode(opcode);
        // get format info from opcode.c

        PC += format;
        if(format == 3 && check_uint8(inst.b1, POS_E)){
            PC += 1;
        }

#ifdef DEBUG
        printf("-----------------------------\n");
        printf("pc : %04X\n", PC);
        printf("inst : %X\n", inst.all);
        printf("disp12 : %03X\n", disp12.all);
        printf("disp20 : %05X\n", disp20.all);
        printf("n : %d\ti : %d\tx : %d\tb : %d\tp : %d\te : %d\t\n"
                , check_uint8(inst.b0, POS_N)
                , check_uint8(inst.b0, POS_I)
                , check_uint8(inst.b1, POS_X)
                , check_uint8(inst.b1, POS_B)
                , check_uint8(inst.b1, POS_P)
                , check_uint8(inst.b1, POS_E)
        );
        fflush(stdout);
#endif

        int n;
        int bit;
        uint32_t TA;
        // target address
        uint32_t TV;
        // target value

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
                        r1 = 0;
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
                    TA = B + disp12.all;
                } else if (!check_uint8(inst.b1, POS_B) && check_uint8(inst.b1, POS_P)) {
                    // pc relative
                    if(check_word(disp12, 20)){
                        disp12.all -= 1;
                        disp12.all ^= set_word(WORD0, 20, 32).all;
                        TA = PC - disp12.all;
                    }
                    else{
                        TA = PC + disp12.all;
                    }
                } else if (opcode == 0x4C) {
                    // RSUB
                    // pass!
                }
                else if (!check_uint8(inst.b0, POS_N)
                    && check_uint8(inst.b0, POS_I)
                       && !check_uint8(inst.b1, POS_X)
                          && !check_uint8(inst.b1, POS_B)
                             && !check_uint8(inst.b1, POS_P)
                                && !check_uint8(inst.b1, POS_E)
                    ){
                    // imm constant
                    // pass!
                    TA = extract_word(inst, 12, 24).all;
                }
                else {
                    printf("not supported addressing!\n");
                }

                if (check_uint8(inst.b1, POS_X)) {
                    TA += X;
                }

#ifdef DEBUG
                printf("target addr is %x\n", TA);
                fflush(stdout);
#endif

                switch (extract_uint8(inst.b0, 6, 8)) {
                    case 0b00000001:
                        // imm addressing
                        TV = TA;
                        break;
                    case 0b00000000:
                    case 0b00000011:
                        // simple addressing
                        TV = deref(TA);
                        break;
                    case 0b00000010:
                        // indirect addressing
                        TA = deref(TA);
                        TV = deref(TA);
                        break;
                    default:
                        // no case
                        break;
                }

#ifdef DEBUG
                printf("Target value is %x\n", TV);
                fflush(stdout);
#endif

                switch (extract_uint8(inst.b0, 0, 6) << 2) {
                    case 0x18:
                        // ADD m          3/4      18    A <-- (A) + (m..m+2)
                        A = A + TV;
                        break;
                    case 0x58:
                        // ADDF m         3/4      58    F <-- (F) + (m..m+5)
                        // TODO
                        break;
                    case 0x40:
                        // AND m          3/4      40    A <-- (A) & (m..m+2
                        A = A & TV;
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
                        A = A / TV;
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
                        L = PC;
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
                        ((word*)(&A))->b3 = ((word*)(&TV))->b1;
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
                        A = A * TV;
                        ((word*)A)->b0 = 0;
                        break;
                    case 0x60:
                        //MULF m         3/4      60    F <-- (F) * (m..m+5)                  X F
                        // TODO
                        break;
                    case 0x44:
                        //OR m           3/4      44    A <-- (A) | (m..m+2)
                        A = A | TV;
                        break;
                    case 0xD8:
                        //RD m           3/4      D8    A [rightmost byte] <-- data         P
                        //                                from device specified by (m)
                        printf("[RD] : %c\n", device_input[cnt_device_input]);
                        A &= set_word(WORD0, 0, 24).all;
                        A |= device_input[cnt_device_input++];
                        break;
                    case 0x4C:
                        //RSUB           3/4      4C    PC <-- (L)
                        PC = L;
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
                                = (uint8_t)A;
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
                        store(TA, L);
                        break;
                    case 0x7C:
                        //STS m          3/4      7C    m..m+2 <-- (S)                        X
                        store(TA, S);
                        break;
                    case 0xE8:
                        //STSW m         3/4      E8    m..m+2 <-- (SW)                     P
                        store(TA, SW);
                        break;
                    case 0x84:
                        //STT m          3/4      84    m..m+2 <-- (T)                        X
                        store(TA, T);
                        break;
                    case 0x10:
                        //STX m          3/4      10    m..m+2 <-- (X)
                        store(TA, X);
                        break;
                    case 0x1C:
                        //SUB m          3/4      1C    A <-- (A) - (m..m+2)
                        A = A - TV;
                        break;
                    case 0x5C:
                        //SUBF m         3/4      5C    F <-- (F) - (m..m+5)                  X F
                        // TODO
                        break;
                    case 0xE0:
                        //TD m           3/4      E0    Test device specified by (m)
                        // IO
                        if(is_input_test_device_ready){
                            SW = 0 - 1;
                            printf("[TD] : success\n");
                        }
                        else{
                            SW = 0;
                            printf("[TD] : failed\n");
                        }
                        break;
                    case 0x2C:
                        //TIX m          3/4      2C    X <-- (X) + 1; (X) : (m..m+2)             C
                        X = X + 1;
                        SW = X - TV;
                        break;
                    case 0xDC:
                        //WD m           3/4      DC    Device specified by (m) <-- (A)[RMB]     P
                        // IO
                        printf("[WD] : %c\n", (uint8_t)A);
                        break;
                    default:
                        printf("not supported opcode!\n");
                        break;

                }
                break;
            default:
                break;
        }
#ifdef DEBUG
        print_reg();
        printf("after TA : %04X\n", TA);
        printf("deref TA : %06X\n", deref(TA));
#endif

        if(PC < progaddr || progaddr + total_length <= PC){
            // out of program
            print_reg();
            printf("End Program\n");
            from_bp = 0;
            return 0;
        }
        else if(exist_bp(PC)){
            // break point hit
            print_reg();
            printf("Stop at checkpoint[%X]\n", PC);
            from_bp = 1;
            return 0;
        }
    }
}