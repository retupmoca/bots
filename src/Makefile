CFLAGS = -g

bots : main.o ops.o machine.o world.o
	gcc -g -o bots -lm main.o ops.o machine.o world.o

clean :
	rm *o bots
