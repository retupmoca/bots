CFLAGS = -g

all : bots asm libbots.so

bots : main.o libbots.so
	gcc -g -o bots -lbots main.o

asm : asm_main.o libbots.so
	gcc -g -o asm -lbots asm_main.o

libbots.so : ops.o machine.o world.o bots.o
	gcc -g -o libbots.so -lm ops.o machine.o world.o bots.o

clean :
	rm -f *o bots asm
