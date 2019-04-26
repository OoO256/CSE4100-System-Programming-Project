#include "history.h"
#include "20171667.h"

void addHistory(char command[MAX_COMMAND_LENGTH]) {
    // add command into history linked list

    string_node* history_new;
    history_new = (string_node *)malloc(sizeof(string_node));
    strcpy(history_new->string, command);
    history_new->next = NULL;
    // make string_node structure


    if (history_head == NULL) {
        // history is empty, add the first node
        history_head = history_new;
    }
    else
    {
        string_node* history_curr = history_head;
        while (history_curr->next != NULL) {
            history_curr = history_curr->next;
        }
        // go to the last node of the history linked list
        history_curr->next = history_new;
        // add history_new at the tail of the linked list
    }
}

void history() {
    // print all history in linked list

    string_node* history_curr = history_head;

    while (history_curr != NULL)
    {
        printf("%s", history_curr->string);
        history_curr = history_curr->next;
    }
}

void freeHistory() {
    // free the memory for history during quitting

    string_node* history_curr = history_head->next;
    string_node* history_prev = history_head;

    while (history_curr != NULL) {
        DEBUG_PRINT(("%s get free!\n", history_prev->string));
        free(history_prev);

        history_prev = history_curr;
        history_curr = history_curr->next;
        // move pointers forward
    }
    // at this line, history_curr == NULL, history_prev != NULL
    // so free history_prev
    DEBUG_PRINT(("%s get free!\n", history_prev->string));
    free(history_prev);
}
