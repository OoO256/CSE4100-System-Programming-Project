#include "20171667.h"
#include "opcode.h"
#include "history.h"
#include "memory.h"
#include "assembler.h"
#include "symbol.h"
#include "linking_loader.h"
#include "run.h"

#define COMMAND_IS(x) (!strcmp(command, (x)))

// for debug
// if we define DEBUG, we can see debug message

int min(int lhs, int rhs) {
    // return minimun interger between lhs, rhs
    if (lhs <= rhs)
        return lhs;
    else
        return rhs;
}

int isHexString(char *str) {
    // check if str is right argument

    char word[MAX_COMMAND_LENGTH];
    memset(word, '\0', MAX_COMMAND_LENGTH);

    int hex;

    int num_arg =
            sscanf(str, "%x %[^ \n]", &hex, word);

    // return false if str contain more than hexadecimal numder
    if (num_arg != 1)
        return false;
    else
        return true;
}

int isOneWord(char *str) {
    // check if str is one word or not

    char word[2][MAX_COMMAND_LENGTH];
    memset(word, '\0', MAX_COMMAND_LENGTH);


    int num_arg =
            sscanf(str, "%[^ \n\t] %[^ \n\t]", word[0], word[1]);

    return num_arg == 1;
}

int hash(char* str){
    // hash string to int [0, 19]
    // add all characters and modular dy 20
    int sum = 0;
    for (char* s = str; *s != '\0'; s++)
    {
        // we ignore space when comparing
        if(*s != ' ')
            sum += *s;
    }
    return sum%20;
}

void help() {
    // do help command, print all possible commands

    char commandList[NUM_COMMAND][50] = {
            "h[elp]",
            "d[ir]",
            "q[uit]",
            "hi[story]",
            "du[mp][start, end]",
            "e[dit] address, value",
            "f[ill] start, end, value",
            "reset",
            "opcode mnemonic",
            "opcodelist",
            "assemble filename",
            "type filename",
            "symbol"
    };

    for (int i = 0; i < NUM_COMMAND; i++) {
        printf("%s\n", commandList[i]);
    }
}

void quit() {
    // do quit, after free all malloced memories
    freeHistory();
    freeHashTable();
    freeSymbols();
    isRunning = false;
}


void dir() {
    // print information of current directory
    DEBUG_PRINT(("jumped into dir()\n"));

    struct stat stat;
    // strcuture represeting status of the information of directory

    DIR *dir;
    struct dirent *ent;
    dir = opendir ("./");
    // dir is currrent directory

    if (dir == NULL) {
        printf("Cannot open current directory!\n");
    }
    else {
        int cnt = 0;
        // count number of file or directory we printed

        while ((ent = readdir (dir)) != NULL) {
            lstat(ent->d_name, &stat);

        if(S_ISDIR (stat.st_mode)){
                // this is directory
            printf("%19s/", ent->d_name);
        }
        else if (S_IXUSR & stat.st_mode) {
            // this means executable
            printf("%19s*", ent->d_name);
        }
        else{
            printf("%20s", ent->d_name);
        }

        cnt++;
        if(cnt%4 == 0)
            printf("\n");
        // print newline for evey 4 files
        }

        if(cnt%4 != 0)
            printf("\n");
            // if not print newline just before, print newline


        closedir (dir);
    }
}

void parseCommand() {
    // get input from stdio
    // and seperate input into command, argments
    // than decide what function should be called or raise error

    printf("sicsim> ");

    char commandLine[MAX_COMMAND_LENGTH];
    fgets(commandLine, MAX_COMMAND_LENGTH, stdin);
    // get whole line

    char command[MAX_COMMAND_LENGTH];
    memset(command, 0, MAX_COMMAND_LENGTH);
    char word[3][MAX_COMMAND_LENGTH];
    memset(word, 0, 3 * MAX_COMMAND_LENGTH);
    char sep[3][MAX_COMMAND_LENGTH];
    memset(sep, 0, 2 * MAX_COMMAND_LENGTH);

    int frontSpace = sscanf(commandLine, "%[ \t]", sep[2]);
    int num_arg;

    if (frontSpace == 0) {
        num_arg = sscanf(commandLine,
                         "%[^ \t\n] %[^,\n] %[,] %[^\n,] %[,] %[^\n]", command, word[0], sep[0], word[1], sep[1],
                         word[2]);
        // seperate commandLine into command, word[0], sep[0], word[1], sep[1], word[2]
        // command get the first word
        // sep[] should be ','
        // word[] get string between command and sep[]
        // num_arg get the number sscanf get from commandLine
    } else {
        num_arg = sscanf(commandLine,
                         "%[ \t] %[^ \t\n] %[^,\n] %[,] %[^\n,] %[,] %[^\n]",
                         sep[2], command, word[0], sep[0], word[1], sep[1], word[2]);
        num_arg--;
    }

    DEBUG_PRINT(("commandLine : %s\n", commandLine));
    DEBUG_PRINT(("num_arg : %d\n", num_arg));

    DEBUG_PRINT(("command : %s\n", command));
    DEBUG_PRINT(("word0 : %s\n", word[0]));
    DEBUG_PRINT(("sep0 : %s\n", sep[0]));
    DEBUG_PRINT(("word1 : %s\n", word[1]));
    DEBUG_PRINT(("sep1 : %s\n", sep[1]));
    DEBUG_PRINT(("word2 : %s\n", word[2]));
    DEBUG_PRINT(("sep2 : %s\n", sep[2]));
    // check if we got right arguments

    switch (num_arg) {
        case 1:
            if (COMMAND_IS("h") || COMMAND_IS("help")) {
                addHistory(commandLine);
                help();
            } else if (COMMAND_IS("d") || COMMAND_IS("dir")) {
                addHistory(commandLine);
                dir();

            } else if (COMMAND_IS("q") || COMMAND_IS("quit")) {
                addHistory(commandLine);
                quit();

            } else if (COMMAND_IS("hi") || COMMAND_IS("history")) {
                addHistory(commandLine);
                history();
            } else if (COMMAND_IS("opcodelist")) {
                opcodelist();
                addHistory(commandLine);
            } else if (COMMAND_IS("reset")) {
                reset();
                addHistory(commandLine);
            } else if (COMMAND_IS("du") || COMMAND_IS("dump")) {
                DEBUG_PRINT(("call dump\n"));

                // call dump [last_dump+1, last_dump+160]
                // if last_dump+160 > 0xFFFFF, call dump [last_dump+1, 0xFFFFF]
                // if last_dump+1 > 0xFFFFF, error will be raised from dump()
                if (dump(last_dump + 1, min(0xFFFFF, last_dump + 1 + 159)) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            }
            else if (COMMAND_IS("symbol")){
                printSymbol();
                addHistory(commandLine);
            }
            else if(COMMAND_IS("run")){
                if (run() == SUCCESSFUL_RETURN){
                    addHistory(commandLine);
                }
            }
            else {
                printf("Syntax error!\n");
            }
            break;
        case 2:
            if (
                    (COMMAND_IS("du") || COMMAND_IS("dump"))
                    && isHexString(word[0])
                    ) {
                // dump start
                int start = (int) strtol(word[0], NULL, 16);
                DEBUG_PRINT(("call dump %02X\n", start));

                if (dump(start, min(0xFFFFF, start + 159)) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            } else if (COMMAND_IS("opcode") && isOneWord(word[0])) {
                //opcode mnemonic
                opcode(word[0]);
                addHistory(commandLine);
            } else if (COMMAND_IS("type")) {
                if (type(word[0]) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            } else if (COMMAND_IS("assemble")) {
                if (assemble(word[0]) == SUCCESSFUL_RETURN) {
                    addHistory(commandLine);
                }
            }
            else if (COMMAND_IS("progaddr") && isHexString(word[0])){
                progaddr = (unsigned int) strtol(word[0], NULL, 16);
                addHistory(commandLine);
            }
            else if (COMMAND_IS("loader")){

                char files[3][100];
                files[0][0] = files[1][0] = files[2][0] = '\0';

                int num_files = sscanf(word[0], "%[^ \n] %[^ \n] %[^ \n]", files[0], files[1], files[2]);

                if(linking_loader(num_files, files[0], files[1], files[2]) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            }
            else {
                printf("Syntax error!\n");
            }
            break;
        case 4:
            if ((COMMAND_IS("du") || COMMAND_IS("dump"))
                && isHexString(word[0])
                && isHexString(word[1])) {

                // for dump start, end

                int start = (int)strtol(word[0], NULL, 16);
                int end = (int)strtol(word[1], NULL, 16);

                DEBUG_PRINT(("call dump %02X %02X\n", start, end));
                if (dump(start, end) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            } else if ((COMMAND_IS("e") || COMMAND_IS("edit"))
                       && isHexString(word[0])
                       && !strcmp(sep[0], ",")
                       && isHexString(word[1])
                    ) {
                // edit address, value
                int address = (int) strtol(word[0], NULL, 16);
                int value = (int) strtol(word[1], NULL, 16);

                DEBUG_PRINT(("address : %02X\n", address));
                DEBUG_PRINT(("value : %02X\n", value));

                DEBUG_PRINT(("call edit\n"));
                if (edit(address, value) == SUCCESSFUL_RETURN)
                    addHistory(commandLine);
            } else {
                printf("Syntax error!\n");
            }
            break;
        case 6:
            if (
                    (COMMAND_IS("f") || COMMAND_IS("fill"))
                && isHexString(word[0])
                && !strcmp(sep[0], ",")
                && isHexString(word[1])
                && !strcmp(sep[1], ",")
                && isHexString(word[2])
                    ) {
                // fill start, end, value
                int start = (int) strtol(word[0], NULL, 16);
                int end = (int) strtol(word[1], NULL, 16);
                int value = (int) strtol(word[2], NULL, 16);

                DEBUG_PRINT(("call fill\n"));
                if (fill(start, end, value) == SUCCESSFUL_RETURN);
                    addHistory(commandLine);
            }
            else {
                printf("Syntax error!\n");
            }
            break;
        default:
            printf("Syntax error!\n");
            break;
    }
}

int main() {
    isRunning = true;
    last_dump = -1;
    history_head = NULL;

    initHashTable();

    while (isRunning) {
        parseCommand();
    }

    return 0;
}
