assembler: main.o fileCompiler.o data.o toBinary.o commandTable.o validation.o
	gcc -g -ansi -pedantic -Wall main.o fileCompiler.o data.o toBinary.o commandTable.o validation.o -o assembler
main.o: main.c fileCompiler.h data.h
	gcc -c -g -ansi -pedantic -Wall main.c -o main.o
fileCompiler.o: fileCompiler.c fileCompiler.h data.h commandTable.h validation.h debugFlags.h
	gcc -c -g -ansi -pedantic -Wall fileCompiler.c -o fileCompiler.o
data.o: data.c data.h commandTable.h debugFlags.h
	gcc -c -g -ansi -pedantic -Wall data.c -o data.o
toBinary.o: toBinary.c toBinary.h commandTable.h fileCompiler.h data.h validation.h
	gcc -c -g -ansi -pedantic -Wall toBinary.c -o toBinary.o
commandTable.o: commandTable.c commandTable.h
	gcc -c -g -ansi -pedantic -Wall commandTable.c -o commandTable.o
validation.o: validation.c validation.h debugFlags.h commandTable.h
	gcc -c -g -ansi -pedantic -Wall validation.c -o validation.o