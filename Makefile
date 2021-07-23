CFLAGS = -g -fPIC -Iinclude

PREFIX = /usr/local
DESTDIR = ""

.PHONY : all cli asm c_lib wasm_lib
all : asm
asm : bin/bots_asm
cli : bin/bots_cli

bin/bots_asm : c_src/asm.o
	$(CC) -g -o bin/bots_asm c_src/asm.o

bin/bots_cli : c_src/cli.o c_lib
	$(CC) -g -o bin/bots_cli c_src/cli.o -Ltarget/debug -lbots

c_lib :
	sed -i.bk 's/\["lib"\]/["lib","cdylib"]/' Cargo.toml
	cargo build --features c_ffi --release
	cbindgen -o bots.h
	mv Cargo.toml.bk Cargo.toml

wasm_lib :
	sed -i.bk 's/\["lib"\]/["lib","cdylib"]/' Cargo.toml
	wasm-pack build --target web -- --features unstable_wasm_ffi
	mv Cargo.toml.bk Cargo.toml

.PHONY : install
install :
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp bin/* ${DESTDIR}/${PREFIX}/bin

.PHONY : clean
clean :
	rm -f c_src/*o bin/bots_asm
