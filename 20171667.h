#pragma once

#define true 1
#define false 0
#define MAX_COMMAND_LENGTH 100
#define NUM_COMMAND 13
#define MEMORY_SIZE (1<<20)
#define MAX_FILE_NAME_LEN 100


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINUX
#ifdef LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

unsigned char memory[MEMORY_SIZE];
// memory of SIC/XE system

enum return_value
        { SUCCESSFUL_RETURN = 0, UNSUCCESSFUL_RETURN };

int isRunning;
int last_dump;

int hash(char* str);

int isHexString(char *str);