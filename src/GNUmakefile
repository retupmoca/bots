CFLAGS = -g -fPIC

all : bots asm libbots.so

bots : main.o libbots.so
	gcc -g -o bots -L. -lbots main.o

asm : asm_main.o libbots.so
	gcc -g -o asm -L. -lbots asm_main.o

libbots.so : ops.o machine.o world.o bots.o
	gcc -g -shared -o libbots.so -lm ops.o machine.o world.o bots.o

clean :
	rm -f *o bots asm
