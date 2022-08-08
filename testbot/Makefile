testbot.bin: testbot.s
	clang --target=riscv32-unknown-elf -march=rv32i testbot.s -nostdlib
	llvm-objcopy -Obinary a.out testbot.bin
	rm a.out
