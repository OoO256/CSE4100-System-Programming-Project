#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linking_loader.h"
#include "vector.h"

unsigned int progaddr = 0;
unsigned int csaddr = 0;
unsigned int execute_addr;

unsigned int hex_from_substring(char* str, int begin, int length){
    char temp[100];
    strncpy(temp, str+begin, length);
    return strtol(temp, NULL, 16);
}

int linker(int num_files, char* file1, char* file2, char* file3){
    char* files[3] = {file1, file2, file3};

    struct vector estab = vector.new();

    for (int file_idx = 0; file_idx < num_files; file_idx++){

        FILE* fp = fopen(files[file_idx], "r");
        if (fp == NULL){
            printf("cannot open %s!\n", files[file_idx]);
        }

        char record;
        char name[6+1];
        unsigned int start_addr, length;
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
                    strncpy(name, line+1, 6);
                    name[6] = '\0';
                    start_addr = hex_from_substring(line, 1+6, 6);
                    length = hex_from_substring(line, 1+6+6, 6);
                    estab.emplace_back(&estab, name, progaddr+csaddr, 1, length);
                    break;
                case 'D':
                    len = strlen(line);
                    num_defs = (strlen(line)-1-1+11)/12;

                    for (int i = 0; i < num_defs; ++i) {
                        strncpy(defs[i], line+1+12*i, 6);
                        defs[i][6] = '\0';
                        addr_defs[i] = hex_from_substring(line, 1+12*i+6, 6);
                        estab.emplace_back(&estab, defs[i], progaddr+csaddr+addr_defs[i], 0, length);
                    }
                    break;
                case 'R':
                    break;
                case 'T':
                    break;
                case 'M':
                    break;
                case 'E':
                    if (strlen(line) >= 1+6+1){
                        execute_addr = hex_from_substring(line, 1, 6);
                    }
                    csaddr += length;
                    break;
                case '.':
                    break;
                default:
                    printf("wrong odj file!\n");
                    return 1;
            }
        }
        estab.print(&estab);
    }
    return 0;
}
