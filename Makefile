CFLAGS = -g -fPIC -Iinclude
CXXFLAGS = -O2 -fPIC -Iinclude -std=c++2a

PREFIX = /usr/local
DESTDIR = ""

LIBDIR = lib

.PHONY : all cli asm lib
all : bin/bots bin/bots_asm bin/bots_gui lib/libbots.so
cli : bin/bots
asm : bin/bots_asm
gui : bin/bots_gui
lib : lib/libbots.so

# just rebuild everything on a header change, since we aren't tracking
# dependencies for each .cpp file
**/*.o : include/**/*.hpp

bin/bots : cli_src/main.o lib/libbots.so
	$(CXX) $(CXXFLAGS) -o bin/bots -Llib cli_src/main.o -lbots -lfmt

bin/bots_asm : asm_src/main.o lib/libbots.so
	$(CC) -g -o bin/bots_asm -Llib asm_src/main.o -lbots

gui_src/main.o : gui_src/*.hpp
bin/bots_gui : gui_src/main.o lib/libbots.so
	$(CXX) $(CXXFLAGS) -o bin/bots_gui -Llib gui_src/main.o -lbots -lfmt -lglfw -lGL -lpthread -lpng

lib/libbots.so : src/ops.o src/cpu.o src/world.o src/peripherals.o
	$(CXX) -g -shared -o lib/libbots.so src/ops.o src/cpu.o src/world.o src/peripherals.o -lm

.PHONY : install
install :
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp bin/* ${DESTDIR}/${PREFIX}/bin
	mkdir -p ${DESTDIR}/${PREFIX}/${LIBDIR}
	cp lib/* ${DESTDIR}/${PREFIX}/${LIBDIR}
	mkdir -p ${DESTDIR}/${PREFIX}/include
	cp -r include/* ${DESTDIR}/${PREFIX}/include

.PHONY : test
test: test/test_runner
	LD_LIBRARY_PATH=lib test/test_runner --verbose

test/test_runner: lib/libbots.so test/cpu_ops.o
	$(CXX) $(CXXFLAGS) -o test/test_runner -Llib test/cpu_ops.o -lbots -lcriterion

.PHONY : clean
clean :
	rm -f asm_src/*o cli_src/*o gui_src/*o src/*o bin/bots bin/bots_asm bin/bots_gui lib/*so test/test_runner test/*o
