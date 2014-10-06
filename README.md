bots
====

I want a modern version of the atrobots programming game, but don't really know what I'm doing.

(http://necrobones.com/atrobots/)

ops

arg types

code | size | name
-----|------|-----
r    |  1   | register
i    |  1   | immediate value
m    |  2   | memory location

number | name | arg1 | arg2 | arg3
-------|------|------|------|------
0|nop
|mov.i | r | i
|mov.r | r | r
|add.rr | r | r | r
|add.ri | r | r | i
|sub.rr | r | r | r
|sub.ri | r | r | i
|mul.rr | r | r | r
|mul.ri | r | r | i
|div.rr | r | r | r
|div.ri | r | r | i
|mod.rr | r | r | r
|mod.ri | r | r | i
|neg | r | r
|or.rr | r | r | r
|or.ri | r | r | i
|and.rr | r | r | r
|and.ri | r | r | i
|xor.rr | r | r | r
|xor.ri | r | r | i
|shl.rr | r | r | r
|shl.ri | r | r | i
|shr.rr | r | r | r
|shr.ri | r | r | i
|not | r | r
|push.r | r
|push.i | i
|pop | r
|put.r | m | r
|put.i | m | i
|get | r | m
|out.r | m | r
|out.i | m | i
|in | r | m
|int.r | r
|int.i | i
|cmp.rr | r | r
|cmp.ri | r | i
|jmp.r | r
|jmp.i | i
|jls.r | r
|jls.i | i
|jgr.r | r
|jgr.i | i
|jne.r | r
|jne.i | i
|jeq.r | r
|jeq.i | i
|jge.r | r
|jge.i | i
|jle.r | r
|jle.i | i
|jz.r | r
|jz.i | i
|jnz.r | r
|jnz.i | i

ports

num | name
----|------
|throttle
|hull steering
|gun steering
|gun hull keepshift
|radar steering (?)
|radar hull keepshift
|radar gun keepshift
|radar arc
|radar target range
|radar target offset
|radar range
|transponder

ints

num|name
-|-
|rset
|destruct
|scan
|fire

