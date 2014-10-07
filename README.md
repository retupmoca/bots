bots
====

I want a modern version of the atrobots programming game, but don't really know what I'm doing.

(http://necrobones.com/atrobots/)

ops

arg types

code | size | name
-----|------|-----
r    |  1   | register
i    |  2   | immediate value

number | name | arg1 | arg2 | arg3
-------|------|------|------|------
0|nop
1 |mov.r | r | r
2 |mov.i | r | i
3 |add.rr | r | r | r
4 |add.ri | r | r | i
5 |sub.rr | r | r | r
6 |sub.ri | r | r | i
7 |mul.rr | r | r | r
8 |mul.ri | r | r | i
9 |div.rr | r | r | r
10 |div.ri | r | r | i
11 |mod.rr | r | r | r
12 |mod.ri | r | r | i
13 |neg | r | r
14 |or.rr | r | r | r
15 |or.ri | r | r | i
16 |and.rr | r | r | r
17 |and.ri | r | r | i
18 |xor.rr | r | r | r
19 |xor.ri | r | r | i
20 |shl.rr | r | r | r
21 |shl.ri | r | r | i
22 |shr.rr | r | r | r
23 |shr.ri | r | r | i
24 |not | r | r
25 |push.r | r
26 |push.i | i
27 |pop | r
28 |put.rr | r | r
29 |put.ri | r | i
30 |put.ir | i | r
31 |put.ii | i | i
32 |get.r | r | r
33 |get.i | r | i
34 |out.rr | r | r
35 |out.ri | r | i
36 |out.ir | i | r
37 |out.ii | i | i
38 |in.r | r | r
39 |in.i | r | i
40 |int.r | r
41 |int.i | i
42 |cmp.rr | r | r
43 |cmp.ri | r | i
44 |jmp.r | r
45 |jmp.i | i
46 |jls.r | r
47 |jls.i | i
48 |jgr.r | r
49 |jgr.i | i
50 |jne.r | r
51 |jne.i | i
52 |jeq.r | r
53 |jeq.i | i
54 |jge.r | r
55 |jge.i | i
56 |jle.r | r
57 |jle.i | i
58 |jz.r | r
59 |jz.i | i
60 |jnz.r | r
61 |jnz.i | i

ports

addr | name
-----|------
0x00 |throttle
0x02 |hull steering
0x04 |gun steering
0x06 |gun hull keepshift
0x08 |radar steering
0x0a |radar hull keepshift
0x0c |radar gun keepshift
0x0e |radar arc
0x10 |radar range
0x12 |radar target range
0x14 |radar target offset
0x16 |transponder

ints

num | name
----|-----
0 |rset
1 |destruct
2 |scan
3 |fire
