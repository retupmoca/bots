CFLAGS = -g -fPIC -Iinclude

PREFIX = /usr/local
DESTDIR = ""

LIBDIR = lib

all : bin/bots bin/bots_asm lib/libbots.so

bin/bots : src/main.o lib/libbots.so
	gcc -g -o bin/bots -Llib src/main.o -lbots

bin/bots_asm : src/asm_main.o lib/libbots.so
	gcc -g -o bin/bots_asm -Llib src/asm_main.o -lbots

lib/libbots.so : src/ops.o src/cpu.o src/world.o src/bots.o
	gcc -g -shared -o lib/libbots.so src/ops.o src/cpu.o src/world.o src/bots.o -lm

install :
	cp bin/* ${DESTDIR}/${PREFIX}/bin
	cp lib/* ${DESTDIR}/${PREFIX}/${LIBDIR}
	cp -r include/* ${DESTDIR}/${PREFIX}/include

clean :
	rm -f src/*o bin/bots bin/bots_asm lib/*so
