#include "20171667.h"
#include "assembler.h"
#include "symbol.h"
#include "opcode.h"

unsigned int regToAddress(char* reg){
    // return address of register
    // if ew cant find, return NO_REGISTER

    if(!strcmp(reg, "")){
        return 0;
    }
    if(!strcmp(reg, "A")){
        return 0;
    }
    else if(!strcmp(reg, "X")){
        return 1;
    }
    else if(!strcmp(reg, "L")){
        return 2;
    }
    else if(!strcmp(reg, "B")){
        return 3;
    }
    else if(!strcmp(reg, "S")){
        return 4;
    }
    else if(!strcmp(reg, "T")){
        return 5;
    }
    else if(!strcmp(reg, "F")){
        return 6;
    }
    else if(!strcmp(reg, "PC")){
        return 8;
    }
    else if(!strcmp(reg, "SW")){
        return 9;
    }
    else {
        return NO_REGISTRE;
    }
}

void raiseError(int line, enum assemble_error error) {
    // handle error
    // print error message

    line *= 5;


    switch (error) {
        case twice_declared:
            fprintf(stdout, "label was declared twice at line %d\n", line);
            break;
        case undefined_mnemonic:
            fprintf(stdout, "undefined mnemonic at line %d\n", line);
            break;
        case undefined_symbol:
            fprintf(stdout, "undefined symbol at line %d\n", line);
            break;
        case undefined_label:
            fprintf(stdout, "undefined label at line %d\n", line);
            break;
        case memory_boundary:
            fprintf(stdout, "out of memory boundary at line %d\n", line);
            break;
        case not_asm_file:
            fprintf(stdout, "this is not .asm file\n");
            break;
        case START_not_exist:
            fprintf(stdout, "START not exist at line %d\n", line);
            break;
    }
}

char* checkX(char* str){
    // check it str have ",X"

    char lable[10], comma[10], x[10];
    int num_arg = sscanf(str, "%[^ ,\n] %[,] %[X]", lable, comma, x);
     if (num_arg == 3){
         return &str[strlen(lable)];
     }
     else {
         return NULL;
     }
}

char* printObj(struct list_line* line){
    // print object code of one line structure

    char* obj = malloc(sizeof(char)* (MAX_OBJ_PER_LINE+1));

    if (line->isCommnet){
        obj[0] = '\0';
    }
    else if(line->obj_size == 0)
        obj[0] = '\0';
    else if(line->data[0] != '\0') {
        strcpy(obj, line->data);
    }
    else{
        char temp_obj[10];
        sprintf(temp_obj, "%X", line->obj);
        if (strlen(temp_obj)%2)
            sprintf(obj, "0");
        sprintf(obj + strlen(temp_obj)%2, "%X", line->obj);
    }
    return obj;
}

void printLine(FILE* stream, struct list_line* line, int idx_line){
    // print one line of list code
    // into FILE* stream

    fprintf(stream, "%-6d ", (idx_line+1)*5);

    if (line->isCommnet){
        fprintf(stream, "      %s", line->str+1);
    }
    else {

        fprintf(stream, "%04X %-6s %-6s %-30s ", line->loc, line->label, line->mnemonic, line->operand);

        if(line->obj_size == 0)
            fprintf(stream, "\n");
        else if(line->data[0] != '\0')
            fprintf(stream, "%s\n", line->data);
        else{
            char obj[10];
            sprintf(obj, "%X", line->obj);
            if (strlen(obj)%2)
                fprintf(stream, "0");
            fprintf(stream, "%X\n", line->obj);
        }
    }
}

int type(char *filename) {
    // do type command
    // print filename file until EOF come

    FILE* fp = fopen(filename, "r");

    if(fp == NULL){
        fprintf(stderr, "cannot open the file");
        return UNSUCCESSFUL_RETURN;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL){
        printf("%s", line);
    }

    printf("\n");
    fclose(fp);
    return SUCCESSFUL_RETURN;
}

int assemble_pass1(FILE* fp, struct list_line list[MAX_LINE_ASM], char codename[10], int* num_line){
    // do pass1
    // push labels to hashTable of symbols
    // count location and save location and information of each line into list[1000] array

    unsigned int loc = 0;
    int line = 0;
    char buf[100];

    while (fgets(buf, 100, fp) != NULL){
        // read one line

        struct list_line* curr = &list[line++];
        // structure for current line

        curr->isCommnet = false;
        curr->loc = loc;
        curr->data[0] = '\0';

        if(buf[0] == '.'){
            curr->isCommnet = true;
            curr->str = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
            strcpy(curr->str, buf);
            continue;
            // if it is comment, save contents of comment
        }
        else if(buf[0] == ' ' || buf[0] == '\t'){
            sscanf(buf, "%s %[^\n]", curr->mnemonic, curr->operand);
            curr->label[0] = '\0';
            // if it start with space or tab, scan mnemonic and operand
        }
        else{
            sscanf(buf, "%s %s %[^\n]", curr->label, curr->mnemonic, curr->operand);

            if(line == 1 && !strcmp(curr->mnemonic, "START")){
                // if current mnemonic is START, do not push into symbol table
            }
            else if (getLoc(curr->label) == NOT_FOUND)
                pushSymbol(curr->label, loc);
            else {
                raiseError(line, twice_declared);
                return UNSUCCESSFUL_RETURN;
            }
        }



        if(line == 1){
            // if there exist START and operand in line 1, set loc = mnemonic, else loc = 0
            // if there is no START at first line, raise error

            if(!strcmp(curr->mnemonic, "START")){
                if(strcmp(curr->operand, "")){
                    loc = (unsigned int)strtol(curr->operand, NULL, 16);
                }
                else{
                    loc = 0;
                }
                list[0].loc = loc;
                strcpy(codename, curr->label);
                curr->obj_size = 0;
            }
            else{
                raiseError(line, START_not_exist);
                return UNSUCCESSFUL_RETURN;
            }
        }
        else if(!strcmp(curr->mnemonic, "END")){

            curr->isCommnet = true;

            curr->str = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
            strcpy(curr->str, buf);
            break;
        }
        else if(!strcmp(curr->mnemonic, "BYTE")){
            // if BYTE copy data into curr-> data
            // and set curr->obj_size to right size


            if(curr->operand[0] == 'X'){

                int length = (int)strlen(curr->operand)-3;
                if(length%2 == 1){
                    sprintf(curr->data, "0");
                }
                // if X'...' have odd number of characters
                // add '0'
                // example : X'E' -> X'0E'

                for (int i = 0; curr->operand[i+2] != '\''; i++) {
                    sprintf(curr->data + i + length%2, "%c", curr->operand[i+2]);
                }

                curr->obj_size = length + length%2;
                loc += length/2;
            }
            else if(curr->operand[0] == 'C') {

                for (int i = 2; curr->operand[i] != '\''; i++) {
                    int c = curr->operand[i];
                    sprintf(curr->data + (i - 2) * 2, "%02X", c);
                }

                curr->obj_size = (int)(2*(strlen(curr->operand)-3));
                loc += strlen(curr->operand)-3;
            }

        }
        else if(!strcmp(curr->mnemonic, "RESB")){
            int length;
            sscanf(curr->operand, "%d", &length);
            loc+=length;

            curr->obj_size = 0;
        }
        else if(!strcmp(curr->mnemonic, "WORD")){
            loc+=3;

            int data;
            sscanf(curr->operand, "%d", &data);
            sprintf(curr->data, "%06X", data);

            curr->obj_size = 3;
        }
        else if(!strcmp(curr->mnemonic, "RESW")){
            int length;
            sscanf(curr->operand, "%d", &length);
            loc+=length*3;

            curr->obj_size=0;
        }
        else if(!strcmp(curr->mnemonic, "BASE")){
            curr->isCommnet = true;

            curr->str = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
            strcpy(curr->str, buf);
        }
        else{

            if(curr->mnemonic[0] == '+'){
                loc += 4;
                curr->obj_size = 4;
            }
            else if (!strcmp(getInst(curr->mnemonic)->format, "1")){
                loc += 1;
                curr->obj_size = 1;
            }
            else if (!strcmp(getInst(curr->mnemonic)->format, "2")){
                loc += 2;
                curr->obj_size = 2;
            }
            else if (!strcmp(getInst(curr->mnemonic)->format, "3/4")){
                loc += 3;
                curr->obj_size = 3;
            }
        }
    }
    *num_line = line;
    return SUCCESSFUL_RETURN;
}

int assemble_pass2(struct list_line list[MAX_LINE_ASM], FILE* fp, int num_line) {
    // do pass2
    // generate obj code of non-data, non-directive instruction

    int B = -1;
    // value of Base Register
    // initialize to -1 so that we can know it is not initialized

    for (int line = 0; line < num_line; line++) {

        struct list_line* curr = &list[line];
        // structure represent curr line

        if(!strcmp(curr->mnemonic, "BASE")){
            B = getLoc(curr->operand);
            // set Base;
        }

        else if (!curr->isCommnet && curr->data[0] == '\0') {
            // for non-data, non-directive instruction

            int format = curr->obj_size;

            if (format == 1) {
                struct format1 obj = {0,};
                obj.opcode = (unsigned int)(getInst(curr->mnemonic)->opcode);

                curr->obj = obj.all;

            } else if (format == 2) {
                struct format2 obj = {0,};
                obj.opcode = (unsigned int)(getInst(curr->mnemonic)->opcode);
                format = 2;


                char r1[10] = {0, }, r2[10] = {0, };
                sscanf(curr->operand, "%[^,] , %s", r1, r2);

                obj.r1 = regToAddress(r1);
                obj.r2 = regToAddress(r2);

                curr->obj = obj.all;

            } else if (format == 3) {
                format = 3;

                struct format3 obj = {0,};
                obj.opcode = (unsigned int)(getInst(curr->mnemonic)->opcode)/4;

                if (!strcmp(curr->mnemonic, "RSUB")){
                    obj.i = 1;
                    obj.n = 1;
                    curr->obj = obj.all;
                }else {

                    char* keep_end_address = checkX(curr->operand);
                    char keep_end_char;

                    if(keep_end_address != NULL) {
                        keep_end_char = *keep_end_address;
                        obj.x = 1;
                        *keep_end_address='\0';
                    }

                    int target_address;
                    if (curr->operand[0] == '@' || curr->operand[0] == '#') {
                        target_address = getLoc(curr->operand + 1);
                    } else {
                        target_address = getLoc(curr->operand);
                    }

                    if (curr->operand[0] == '#') {
                        obj.i = 1;
                    } else if (curr->operand[0] == '@') {
                        obj.n = 1;
                    } else {
                        obj.i = 1;
                        obj.n = 1;
                    }

                    int pc = curr->loc + 3;
                    if (curr->operand[0] == '#' && getLoc(curr->operand + 1) == NOT_FOUND)
                    {
                        // for constant value
                        obj.disp = (unsigned int)strtol(curr->operand + 1, NULL, 10);

                    } else if (-2048 <= target_address - pc && target_address - pc <= 2047) {
                        // for PC relative
                        obj.b = 0;
                        obj.p = 1;
                        obj.disp = (target_address - pc);

                    } else if (B != -1 && 0 <= target_address - B && target_address - B <= 4095) {
                        // for Base relative
                        obj.b = 1;
                        obj.p = 0;
                        obj.disp = (target_address - B);
                    } else {
                        // cannot represent address in format3
                        raiseError(line, memory_boundary);
                        return UNSUCCESSFUL_RETURN;
                    }
                    curr->obj = obj.all;

                    if(keep_end_address != NULL) {
                        *keep_end_address = keep_end_char;
                    }
                }
            }
            if (format == 4) {

                struct format4 obj = {0,};
                obj.e = 1;
                obj.opcode = getInst(curr->mnemonic + 1)->opcode/4;

                char* keep_end_address = checkX(curr->operand);
                char keep_end_char;

                if(keep_end_address != NULL) {
                    keep_end_char = *keep_end_address;
                    obj.x = 1;
                    *keep_end_address='\0';
                }

                if (curr->operand[0] == '#') {
                    if(getLoc(curr->operand + 1) == NOT_FOUND)
                        obj.address = strtol(curr->operand + 1, NULL, 10);
                    else
                        obj.address = getLoc(curr->operand + 1);
                    obj.i = 1;
                } else if (curr->operand[0] == '@') {
                    obj.n = 1;
                    obj.address = getLoc(curr->operand + 1);
                } else {
                    obj.i = 1;
                    obj.n = 1;
                    obj.address = getLoc(curr->operand);
                }

                if(obj.x == 1){
                    *strchr(curr->operand, '\0') = ',';
                }
                curr->obj = obj.all;

                if(keep_end_address != NULL) {
                    *keep_end_address = keep_end_char;
                }
            }
        }
        printLine(fp, curr, line);
    }
    return SUCCESSFUL_RETURN;
}

void writeObj(struct list_line list[MAX_LINE_ASM], FILE* fp, int num_line, char* codename) {
    // write object code

    fprintf(fp, "H%-6s%06X%06X\n", codename, list[0].loc, list[num_line - 1].loc - list[0].loc);

    int line = 1;

    do {

        char buf[70] = {0,};
        int place_print = 0;

        do {
            char *obj = printObj(&list[line]);
            int obj_length = strlen(obj);
            if (place_print + obj_length > 60)
                break;

            if (list[line].obj_size == 0 && list[line].isCommnet == 0) {
                // for variables or constants
                line++;
                break;
            }

            sprintf(buf + place_print, "%s", obj);
            place_print += obj_length;
            line++;
        } while (line < num_line);

        if (buf[0] != '\0')
            fprintf(fp, "T%06X%02X%s\n", list[line].loc, (int)strlen(buf), buf);
    } while (line < num_line);

    for (int i = 0; i < num_line; ++i) {

        if (!list[i].isCommnet && list[i].data[0] == '\0' && list[i].obj_size == 4 &&
            !(list[i].operand[0] == '#' && getLoc(list[i].operand + 1) == NOT_FOUND)) {
            // for format 4
            // except #constants
            fprintf(fp, "M%06X%02X\n", list[i].loc - list[0].loc + 1, 05);
        }
    }
    fprintf(fp, "E%06X\n", list[0].loc);
}

int assemble(char* filename_asm){
    int filename_length = strlen(filename_asm);
    if(filename_length < 4
       || strcmp(&filename_asm[filename_length-4], ".asm")
            ){

        raiseError(-1, not_asm_file);
        return UNSUCCESSFUL_RETURN;
    }


    FILE* fp_asm = fopen(filename_asm, "r");

    if(fp_asm == NULL){
        fprintf(stderr, "cannot open the file");
        return UNSUCCESSFUL_RETURN;
    }

    initSymbolTable();
    char codename[10];
    struct list_line list[MAX_LINE_ASM];

    int num_line = 0;
    int return_pass1 = assemble_pass1(fp_asm, list, codename, &num_line);
    fclose(fp_asm);
    if (return_pass1 == UNSUCCESSFUL_RETURN){
        // if error raised during pass1, exit function
        return UNSUCCESSFUL_RETURN;
    }


    char filename_lst[100];
    strcpy(filename_lst, filename_asm);
    strcpy(strchr(filename_lst, '.'), ".lst");


    char filename_obj[100];
    strcpy(filename_obj, filename_asm);
    strcpy(strchr(filename_obj, '.'), ".obj");


    DEBUG_PRINT(("loc  label  opcode format\n"));


    FILE* fp_lst =  fopen(filename_lst, "w");
    int return_pass2 = assemble_pass2(list, fp_lst, num_line);
    fclose(fp_lst);
    if (return_pass2 == UNSUCCESSFUL_RETURN){
        // if error raised during pass2, delete lst and exit function
        remove(filename_lst);
        return UNSUCCESSFUL_RETURN;
    }

    FILE* fp_obj =  fopen(filename_obj, "w");
    writeObj(list, fp_obj, num_line, codename);
    fclose(fp_obj);
    return SUCCESSFUL_RETURN;
}
