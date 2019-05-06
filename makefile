20171667.out : 20171667.o opcode.o assembler.o history.o memory.o symbol.o vector.o linking_loader.o run.o breakpoints.o
	gcc -std=gnu99 -Wall 20171667.o opcode.o assembler.o history.o memory.o symbol.o vector.o linking_loader.o run.o breakpoints.o -o 20171667.out
	
20171667.o : 20171667.c
	gcc -c -std=gnu99 -Wall 20171667.c
	
opcode.o : opcode.c
	gcc -c -std=gnu99 -Wall opcode.c
	
assembler.o : assembler.c
	gcc -c -std=gnu99 -Wall assembler.c
	
history.o : history.c
	gcc -c -std=gnu99 -Wall history.c
	
memory.o : memory.c
	gcc -c -std=gnu99 -Wall memory.c
	
symbol.o : symbol.c
	gcc -c -std=gnu99 -Wall symbol.c
	
vector.o : vector.c
	gcc -c -std=gnu99 -Wall vector.c
	
linking_loader.o : linking_loader.c
	gcc -c -std=gnu99 -Wall linking_loader.c
	
run.o : run.c
	gcc -c -std=gnu99 -Wall run.c

breakpoints.o : breakpoints.c
	gcc -c -std=gnu99 -Wall breakpoints.c

clean :
	rm *.o 20171667.out
