#include "opcode.h"


//#define DEBUG
#ifdef DEBUG
	#define DEBUG_PRINT(x) printf x
#else
	#define DEBUG_PRINT(x) do {} while (0)
#endif
// for debug
// if we define DEBUG, we can see debug message

void pushToHash(node_instruction* inst)
{
    // push instruction to hashtable
    int target_idx = hash(inst->mnemonic);
    
    if (hashTable[target_idx] == NULL)
    {
        hashTable[target_idx] = inst;
    }
    else
    {
        // if hashTable[target_idx] is not empty, 
        // add inst at tail of the linked list
        node_instruction* curr = hashTable[target_idx];
        while (curr->next != NULL){
            curr = curr->next;
        }
        
        curr->next = inst;
    }
}

void initHashTable()
{
    // make hashtable fron opcode.txt
    hashTable = (node_instruction**)calloc(20, sizeof(node_instruction*));
	
    FILE *fp = fopen("opcode.txt", "r");
    if (fp == NULL)
        printf("file open failed!\n");
    
    int opcode;
    char mnemonic[MAX_COMMAND_LENGTH];
    char operand[MAX_COMMAND_LENGTH];
    
    while (fscanf(fp, "%X %s %s", &opcode, mnemonic , operand) != EOF)
    {
        node_instruction* curr_instruction = malloc(sizeof(node_instruction));
        curr_instruction->opcode = opcode;
        strcpy(curr_instruction->mnemonic, mnemonic);
        strcpy(curr_instruction->format, operand);
        curr_instruction->next = NULL;
        
        pushToHash(curr_instruction);
    }
	fclose(fp);
}

void opcodelist(){
    // print hashtable
    for (int i = 0; i < LENGTH_HASHTABLE; i++){
        printf("%d : ", i);
        
        if (hashTable[i] == NULL)
        {
            // this means empty
            printf("\n");
        }
        else 
        {
            node_instruction* curr = hashTable[i];
            //print linked list 
            
            while(curr->next != NULL){
                printf("[%s, %02X] -> ", curr->mnemonic, curr->opcode);
                curr = curr->next;
            }
            printf("[%s, %02X]\n", curr->mnemonic, curr->opcode);
            // for the last node, we do not print " -> "
        }
    }
}

void freeHashTable(){
    // free memories of HashTable before exit program
    
	for (int i = 0; i < LENGTH_HASHTABLE; i++)
	{
		DEBUG_PRINT(("i is %d\n", i));
        
        if (hashTable[i] != NULL)
        {
            node_instruction* curr = hashTable[i]->next;
            node_instruction* prev = hashTable[i];
            
            while (curr != NULL) {
		        DEBUG_PRINT(("%s get free!\n", prev->mnemonic));
		        free(prev);

		        prev = curr;
		        curr = curr->next;		
	        }
	        DEBUG_PRINT(("%s get free!\n", prev->mnemonic));
	        free(prev);
        }
    }
	free(hashTable);
}

char* removeSpaces(char* str)
{
    // remove space from str so that find 
    char* i = str;
    char* j = str;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = '\0';
    return str;
}

node_instruction* getInst(char* mnemonic) {
    int hash_address = hash(removeSpaces(mnemonic));
    // get hash output

    if (hashTable[hash_address] == NULL) {
        return NULL;
    } else {
        node_instruction *curr = hashTable[hash_address];

        while (curr != NULL) {
            if (!strcmp(mnemonic, curr->mnemonic)) {
                // check if they are equal
                return curr;
            }
            curr = curr->next;
        }
        return NULL;
    }
}


int opcode(char* mnemonic){
    // print opcode of mnemonic
    // if cannot, print Cannot Find

    node_instruction* inst = getInst(mnemonic);

    if(inst == NULL) {
        printf("Cannot Find %s\n", mnemonic);
        return UNSUCCESSFUL_RETURN;
    }
    else {
        printf("opcode is %02X\n", inst->opcode);
        return SUCCESSFUL_RETURN;
    }
}