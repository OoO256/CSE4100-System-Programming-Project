#ifndef CSE4100_SYSTEM_PROGRAMMING_PROJECT_1_HISTORY_H
#define CSE4100_SYSTEM_PROGRAMMING_PROJECT_1_HISTORY_H

#include "20171667.h"
typedef struct _string_node {
    char string[MAX_COMMAND_LENGTH];
    struct _string_node *next;
} string_node;

string_node* history_head;

void addHistory(char command[MAX_COMMAND_LENGTH]);

void history();

void freeHistory();

#endif //CSE4100_SYSTEM_PROGRAMMING_PROJECT_1_HISTORY_H
