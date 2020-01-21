CFLAGS = -g -fPIC -Iinclude
CXXFLAGS = -g -fPIC -Iinclude -std=c++2a

PREFIX = /usr/local
DESTDIR = ""

LIBDIR = lib

.PHONY : all cli asm lib
all : bin/bots bin/bots_asm lib/libbots.so
cli : bin/bots
asm : bin/bots_asm
lib : lib/libbots.so

# just rebuild everything on a header change, since we aren't tracking
# dependencies for each .c file
**/*.o : include/**/*.h

bin/bots : cli_src/main.o lib/libbots.so
	$(CXX) $(CXXFLAGS) -o bin/bots -Llib cli_src/main.o -lbots -lfmt

bin/bots_asm : asm_src/main.o lib/libbots.so
	$(CC) -g -o bin/bots_asm -Llib asm_src/main.o -lbots

lib/libbots.so : src/ops.o src/cpu.o src/world.o src/bots.o src/peripherals.o
	$(CC) -g -shared -o lib/libbots.so src/ops.o src/cpu.o src/world.o src/bots.o src/peripherals.o -lm

.PHONY : install
install :
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp bin/* ${DESTDIR}/${PREFIX}/bin
	mkdir -p ${DESTDIR}/${PREFIX}/${LIBDIR}
	cp lib/* ${DESTDIR}/${PREFIX}/${LIBDIR}
	mkdir -p ${DESTDIR}/${PREFIX}/include
	cp -r include/* ${DESTDIR}/${PREFIX}/include

.PHONY : clean
clean :
	rm -f asm_src/*o cli_src/*o src/*o bin/bots bin/bots_asm lib/*so
