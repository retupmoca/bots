testbot.bin: testbot.s
	clang --target=riscv32-unknown-elf -march=rv32i testbot.s -nostdlib
	riscv32-unknown-elf-objcopy -Obinary a.out testbot.bin
	rm a.out
