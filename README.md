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
7 |sub.ir | r | i | r
8 |mul.rr | r | r | r
9 |mul.ri | r | r | i
10 |div.rr | r | r | r
11 |div.ri | r | r | i
12 |div.ir | r | r | i
13 |mod.rr | r | r | r
14 |mod.ri | r | r | i
15 |mod.ir | r | i | r
16 |neg | r | r
17 |or.rr | r | r | r
18 |or.ri | r | r | i
19 |and.rr | r | r | r
20 |and.ri | r | r | i
21 |xor.rr | r | r | r
22 |xor.ri | r | r | i
23 |shl.rr | r | r | r
24 |shl.ri | r | r | i
25 |shl.ir | r | i | r
26 |shr.rr | r | r | r
27 |shr.ri | r | r | i
28 |shr.ir | r | i | r
29 |not | r | r
30 |push.r | r
31 |push.i | i
32 |pop | r
33 |put.rr | r | r
34 |put.ri | r | i
35 |put.ir | i | r
36 |put.ii | i | i
37 |get.r | r | r
38 |get.i | r | i
39 |out.rr | r | r
40 |out.ri | r | i
41 |out.ir | i | r
42 |out.ii | i | i
43 |in.r | r | r
44 |in.i | r | i
45 |int.r | r
46 |int.i | i
47 |cmp.rr | r | r
48 |cmp.ri | r | i
49 |cmp.ir | i | r
50 |jmp.r | r
51 |jmp.i | i
52 |jls.r | r
53 |jls.i | i
54 |jgr.r | r
55 |jgr.i | i
56 |jne.r | r
57 |jne.i | i
58 |jeq.r | r
59 |jeq.i | i
60 |jge.r | r
61 |jge.i | i
62 |jle.r | r
63 |jle.i | i
64 |jz.r | r
65 |jz.i | i
66 |jnz.r | r
67 |jnz.i | i

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
