#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linking_loader.h"
#include "vector.h"
#include "memory.h"
#include "20171667.h"

unsigned int progaddr = 0;
unsigned int execute_addr;
int total_length;
struct vector estab;

unsigned int hex_from_substring(char* str, int begin, int length){
    char temp[100];
    strncpy(temp, str+begin, length);
    temp[length] = '\0';
    return strtol(temp, NULL, 16);
}
// extract hex number from substring

int linking_loader(int num_files, char* file1, char* file2, char* file3){
    total_length = 0;
    estab = vector.new();
    char* files[3] = {file1, file2, file3};
    linker_pass1(num_files, files);
    linker_pass2(num_files, files);
}
// main function of linking_loader

int linker_pass1(int num_files, char **files){
    unsigned int csaddr = 0;
    for (int file_idx = 0; file_idx < num_files; file_idx++){

        FILE* fp = fopen(files[file_idx], "r");
        if (fp == NULL){
            printf("cannot open %s!\n", files[file_idx]);
        }

        char record;
        char name[6+1];
        unsigned int start_addr, cs_length;
        char line[100];
        line[0] = '0';
        while (fgets(line, 100, fp) != NULL){
            char defs[6][7];
            unsigned int addr_defs[6];
            int num_defs, num_end_args, end_addr;
            int len;

            record = line[0];

            switch (record){

                case 'H':
                    // in H record, set name of program, cs_length, insert name to estab
                    strncpy(name, line+1, 6);
                    name[6] = '\0';
                    start_addr = hex_from_substring(line, 1+6, 6);
                    cs_length = hex_from_substring(line, 1+6+6, 6);
                    estab.emplace_back(&estab, name, progaddr+csaddr, 1, cs_length);
                    break;
                case 'D':
                    // in D record, insert symbol to estab
                    len = strlen(line);
                    num_defs = (strlen(line)-1-1+11)/12;

                    for (int i = 0; i < num_defs; ++i) {
                        strncpy(defs[i], line+1+12*i, 6);
                        defs[i][6] = '\0';
                        addr_defs[i] = hex_from_substring(line, 1+12*i+6, 6);
                        estab.emplace_back(&estab, defs[i], progaddr+csaddr+addr_defs[i], 0, cs_length);
                    }
                    break;
                case 'R':
                    break;
                case 'T':
                    break;
                case 'M':
                    break;
                case 'E':
                    // in E record, set execute addr and increase csaddr
                    if (strlen(line) >= 1+6+1){
                        execute_addr = hex_from_substring(line, 1, 6);
                    }
                    csaddr += cs_length;
                    break;
                case '.':
                    break;
                default:
                    printf("wrong odj file!\n");
                    return 1;
            }
        }
    }
    estab.print(&estab);
    // print estab
    total_length = csaddr;
    // total length of program is the last csaddr value
    return 0;
}

int linker_pass2(int num_files, char **files){
    // linker_pass2
    unsigned int csaddr = 0;
    for (int file_idx = 0; file_idx < num_files; file_idx++){

        struct vector local_estab = vector.new();
        // for each control section, we have local estab for reference

        FILE* fp = fopen(files[file_idx], "r");
        if (fp == NULL){
            printf("cannot open %s!\n", files[file_idx]);
        }

        char record;
        char name[6+1];
        unsigned int start_addr, length, cs_length;
        char line[100];
        line[0] = '0';
        while (fgets(line, 100, fp) != NULL){
            char defs[6][7];
            unsigned int addr_defs[6];
            int num_defs, num_end_args, end_addr;
            int len;
            int reference_number, diff;
            int to_edit, sign;


            record = line[0];

            switch (record){

                case 'H':
                    // in H record, set name of program, cs_length, insert name to local estab
                    strncpy(name, line+1, 6);
                    name[6] = '\0';
                    start_addr = hex_from_substring(line, 1+6, 6);
                    cs_length = hex_from_substring(line, 1+6+6, 6);

                    local_estab.emplace_back(&local_estab, name,
                                             estab.find(&estab, name)->addr, 1, 0);
                    break;
                case 'D':
                    break;
                case 'R':
                    // push extref from estab to local estab
                    for (int i = 0; 1+8*i+2 < strlen(line); ++i) {
                        strncpy(name, line+1+8*i+2, 6);
                        name[6] = '\0';

                        local_estab.emplace_back(&local_estab, name,
                                estab.find(&estab, name)->addr, 0, 0);
                    }
                    
                    break;
                case 'T':
                    // load text
                    start_addr = hex_from_substring(line, 1, 6) + csaddr + progaddr;
                    length = hex_from_substring(line, 1+6, 2);
                    for (int i = 0; i < length; ++i) {
                        memory[start_addr+i] = hex_from_substring(line+1+6+2, 2*i, 2);
                    }
                    break;
                case 'M':
                    // modify symbol form local estab
                    fflush(stdout);
                    start_addr = hex_from_substring(line, 1, 6) + csaddr + progaddr;
                    length = 6;

                    reference_number = atoi(line+10);
                    if (local_estab.get(&local_estab, reference_number) != NULL)
                        diff = local_estab.get(&local_estab, reference_number)->addr;
                    else
                        printf("!!!!!!!!!!!!!!!!!!!");

                    to_edit = ((int)memory[start_addr])*(1<<16)
                            + ((int)memory[start_addr+1])*(1<<8)
                            + memory[start_addr+2];
                    if(line[9] == '+')
                        to_edit += diff;
                    else if (line[9] == '-')
                        to_edit -= diff;


                    memory[start_addr+2] = (unsigned char)to_edit;
                    to_edit >>= 8;
                    memory[start_addr+1] = (unsigned char)to_edit;
                    to_edit >>= 8;
                    memory[start_addr] = (unsigned char)to_edit;

                    break;
                case 'E':
                    //local_estab.print(&local_estab);

                    if (strlen(line) >= 1+6+1){
                        execute_addr = hex_from_substring(line, 1, 6);
                    }
                    csaddr += cs_length;
                    break;
                case '.':
                    break;
                default:
                    printf("wrong odj file!\n");
                    return 1;
            }
        }
    }
    return 0;
}




