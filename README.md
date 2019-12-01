# bots


I want a modern version of the atrobots programming game, but don't really know what I'm doing.

(http://necrobones.com/atrobots/)

# ops

Memory layout per instruction:
4 bit opcode
4 bit opcode flags (immediate vs register argument, shift direction, etc)
4 bit register a
4 bit register b
16 bit immediate (possibly used as another register number)

number | name | cycles | arg1 | arg2 | arg3
-------|------|--------|------|------|-----
0      | nop  | 1
1      | add  | 1      | r    | r    | r/i
2      | sub  | 1      | r    | r/i  | r/i
3      | mul |2| r | r | r/i
4      | div |2| r | r/i | r/i
5      | or |1| r | r | r/i
6      | and |1| r | r | r/i
7      | xor |1| r | r | r/i
8      | shift |1| r | r/i | r/i
9      | not |1| r | r
10     | push |1| r
11     | pop |1| r
12     | store |1| r | r/i | r
13     | load |1| r | r | r/i
14     | cmp |1| r | r/i
15     | jmp |1/2| r | r/i

## nop

Does nothing. Takes no arguments.

## add, mul, or, and, xor

    add r2 r3 r4 ; r2 = r3 <op> r4
    add r2 r3 5 ; r2 = r3 <op> 5

## sub

    sub r2 r3 r4 ; r2 = r3 - r4
    sub r2 r3 5 ; r2 = r3 - 5
    sub r2 5 r3 ; r2 = 5 - r3

## div

    div r2 r3 r4 ; r2 = r3 / r4
    div r2 r3 5 ; r2 = r3 / 5
    div r2 5 r3 ; r2 = 5 / r3
    div.% r2 r3 r4 ; r2 = r3 % r4
    div.% r2 r3 5 ; r2 = r3 % 5
    div.% r2 5 r3 ; r2 = 5 % r3

## shift

    shift r2 r3 r4 ; r2 = r3 >> r4
    shift r2 r3 5 ; r2 = r3 >> 5
    shift r2 5 r3 ; r2 = 5 >> r3
    shift.< r2 r3 r4 ; r2 = r3 << r4
    shift.< r2 r3 5 ; r2 = r3 << 5
    shift.< r2 5 r3 ; r2 = 5 << r3

## not

    not r2 r3 ; r2 = ~r3

## push/pop

    push r2 ; pushes the value in r2 onto stack
    pop r2 ; pops the value from stack into r2

## store/load

    store r2 r3 r4 ; stores a 2-byte word: mem[r2+r3] = r4
    store.b r2 r3 r4 ; stores a single byte: mem[r2+r3] = r4
    store r2 123 r4 ; stores a 2-byte word: mem[r2+123] = r4
    store.b r2 123 r4 ; stores a single byte: mem[r2+123] = r4

    load r4 r2 r3 ; loads a 2-byte word: r4 = mem[r2+r3]
    load.b r4 r2 r3 ; loads a single byte: r4 = mem[r2+r3]
    load r4 r2 123 ; loads a 2-byte word: r4 = mem[r2+123]
    load.b r4 r2 123 ; loads a single byte: r4 = mem[r2+123]

## cmp

    cmp r2 r3 ; set flags based on r2 and r3
    cmp r2 5 ; set flags based on r2 and 5
    cmp 5 r2 ; set flags based on 5 and r2

## jmp
    
    jmp r2 123 ; pc = r2+123
    jmp.< ... ; jump if less than
    jmp.= ... ; jump if equal
    jmp.> ... ; jump if greater than
    jmp.! ... ; jump if not equal
    jmp.c ... ; call: push pc onto stack before jumping

# registers

reg | for
----|----
0 | always 0
1 | always 1
2 | general
3 | general
4 | general
5 | general
6 | general
7 | general
8 | general
9 | general
10 | stack pointer (points to maximum user memory on startup, grows down)
11 | flags

# memory map

addr hex | addr dec | size | what
---------|----------|------|-----
0xffff | 65535 | b | turret: fire
0xfffd | 65533 | w | turret: reserved
0xfffb | 65531 | w | turret: steering
0xfffa | 65530 | b | turret: keepshift
0xfff8 | 65528 | w | reserved
0xfff6 | 65526 | w | reserved
0xfff5 | 65525 | b | radar: scan
0xfff3 | 65523 | w | radar: range
0xfff2 | 65522 | b | radar: arc
0xfff0 | 65520 | w | radar: steering
0xffef | 65519 | b | radar: keepshift
0xffee | 65518 | b | radar: reserved
0xffec | 65516 | w | radar result: range
0xffea | 65514 | w | radar result: offset
0xffe8 | 65512 | w | reserved
0xffe6 | 65510 | w | reserved
0xffe5 | 65509 | b | nav: throttle
0xffe4 | 65508 | b | nav: reserved
0xffe2 | 65506 | w | nav: reserved
0xffe0 | 65504 | w | nav: steering
0xffde | 65502 | w | reserved
0xffdc | 65500 | w | reserved
0xffdb | 65499 | b | trigger reset
0xffda | 65498 | b | trigger self destruct
0xefff | 61439 | - | limit of program space
