CFLAGS = -g -fPIC -Iinclude

PREFIX = /usr/local
DESTDIR = ""

.PHONY : all cli asm
all : cli asm
asm : bin/bots_asm
cli : bin/bots_cli

bin/bots_asm : c_src/asm.o
	$(CC) -g -o bin/bots_asm c_src/asm.o

bin/bots_cli : c_src/cli.o
	$(CC) -g -o bin/bots_cli c_src/cli.o -Ltarget/debug -lbots

.PHONY : install
install :
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp bin/* ${DESTDIR}/${PREFIX}/bin

.PHONY : clean
clean :
	rm -f c_src/*o bin/bots_asm
