CFLAGS = -g

all : bots asm

bots : main.o ops.o machine.o world.o
	gcc -g -o bots -lm main.o ops.o machine.o world.o

asm : asm_main.o ops.o
	gcc -g -o asm asm_main.o ops.o

clean :
	rm -f *o bots asm
