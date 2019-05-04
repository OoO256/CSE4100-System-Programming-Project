#ifndef PROJECT_LINKING_LOADER_H
#define PROJECT_LINKING_LOADER_H

extern unsigned int progaddr;

int linker_pass1(int num_files, char **files);
int linker_pass2(int num_files, char **files);
int linking_loader(int num_files, char* file1, char* file2, char* file3);


extern unsigned int progaddr;
extern unsigned int execute_addr;
extern int total_length;
#endif //PROJECT_LINKING_LOADER_H
