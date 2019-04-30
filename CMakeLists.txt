ADD_EXECUTABLE(
        20171667.out
        20171667.c
        assembler.c
        history.c
        memory.c
        opcode.c
        symbol.c
        linker.cpp linker.h hashtable.h hashtable.c)

set(RESOURCE_FILES
        opcode.txt
        )