# bots

I want a modern version of the atrobots programming game, but don't really know what I'm doing.

(http://necrobones.com/atrobots/)

# units

Consider one "distance unit" to be one mm

Consider one "world tick" to be 1/60th of a second(?)

# CPU

This simulates a risc-v CPU with the basic rv32i instruction set

# memory map

addr hex | addr dec | size | what
---------|----------|------|-----
0xf0000030 | 65264 | b | trigger reset
|||
0xf000002b | 65259 | w | radar: target offset
0xf0000029 | 65257 | w | radar: steering
0xf0000028 | 65256 | b | radar: scan
0xf0000026 | 65254 | w | radar: range
0xf0000025 | 65253 | b | radar: arc
0xf0000024 | 65252 | b | radar: keepshift
0xf0000022 | 65250 | w | radar: result range
0xf0000020 | 65248 | w | radar: result offset
|||
0xf0000014 | 65236 | w | turret: target offset
0xf0000012 | 65234 | w | turret: steering
0xf0000011 | 65233 | b | turret: keepshift
0xf0000010 | 65232 | b | turret: fire
|||
0xf0000001 | 65217 | w | hull: steering
0xf0000000 | 65216 | b | hull: throttle
|||
0xefff | 61439 | - | limit of program space
