PREFIX = /usr/local
CFLAGS = -g -fPIC -Iinclude

all : bin/bots bin/asm lib/libbots.so

bin/bots : src/main.o lib/libbots.so
	gcc -g -o bin/bots -Llib -lbots src/main.o

bin/asm : src/asm_main.o lib/libbots.so
	gcc -g -o bin/asm -Llib -lbots src/asm_main.o

lib/libbots.so : src/ops.o src/machine.o src/world.o src/bots.o
	gcc -g -shared -o lib/libbots.so -lm src/ops.o src/machine.o src/world.o src/bots.o

install :
	cp bin/* $PREFIX/bin
	cp lib/* $PREFIX/lib
	cp -r include/* $PREFIX/include

clean :
	rm -f src/*o bin/bots bin/asm lib/*so
