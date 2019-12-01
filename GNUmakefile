PREFIX = /usr/local
CFLAGS = -g -fPIC -Iinclude

all : bin/bots bin/bots_asm lib/libbots.so

bin/bots : src/main.o lib/libbots.so
	gcc -g -o bin/bots -Llib src/main.o -lbots

bin/bots_asm : src/asm_main.o lib/libbots.so
	gcc -g -o bin/bots_asm -Llib src/asm_main.o -lbots

lib/libbots.so : src/ops.o src/cpu.o src/world.o src/bots.o src/peripherals.o
	gcc -g -shared -o lib/libbots.so src/ops.o src/cpu.o src/world.o src/bots.o src/peripherals.o -lm

install :
	cp bin/* ${PREFIX}/bin
	cp lib/* ${PREFIX}/lib
	cp -r include/* ${PREFIX}/include

clean :
	rm -f src/*o bin/bots bin/bots_asm lib/*so
