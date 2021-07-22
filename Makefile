CFLAGS = -g -fPIC -Iinclude

PREFIX = /usr/local
DESTDIR = ""

.PHONY : asm
asm : bin/bots_asm

bin/bots_asm : asm_src/main.o
	$(CC) -g -o bin/bots_asm asm_src/main.o

.PHONY : install
install :
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp bin/* ${DESTDIR}/${PREFIX}/bin

.PHONY : clean
clean :
	rm -f asm_src/*o bin/bots_asm
