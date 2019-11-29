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
33 |sw.rrr | r | r | r
34 |sb.rrr | r | r | r
35 |sw.rir | r | i | r
36 |sb.rir | r | i | r
37 |lw.rrr | r | r | r
38 |lb.rrr | r | r | r
39 |lw.rri | r | r | i
40 |lb.rri | r | r | i
41 |out.rr | r | r
42 |out.ri | r | i
43 |out.ir | i | r
44 |out.ii | i | i
45 |in.r | r | r
46 |in.i | r | i
47 |int.r | r
48 |int.i | i
49 |cmp.rr | r | r
50 |cmp.ri | r | i
51 |cmp.ir | i | r
52 |jmp.r | r
53 |jmp.i | i
54 |jls.r | r
55 |jls.i | i
56 |jgr.r | r
57 |jgr.i | i
58 |jne.r | r
59 |jne.i | i
60 |jeq.r | r
61 |jeq.i | i
62 |jge.r | r
63 |jge.i | i
64 |jle.r | r
65 |jle.i | i
66 |jz.r | r
67 |jz.i | i
68 |jnz.r | r
69 |jnz.i | i
70 |call.r | r
71 |call.i | i
72 |ret

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
0x18 |hull steering adjust
0x1a |gun steering adjust

ints

num | name
----|-----
0 |rset
1 |destruct
2 |scan
3 |fire
