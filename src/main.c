#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "machine.h"
#include "world.h"
#include "ops.h"

void main() {
    world w = { 0 };
    machine m1 = { 0 };
    bot_physics mp1 = { 0 };
    mp1.x = 25;
    mp1.y = 50;

    /* test program */
    /* out 3 127 */
    m1.memory[0] = 4;
    m1.memory[1] = 3;
    m1.memory[2] = 127;

    /* out 4 5 */
    m1.memory[3] = 4;
    m1.memory[4] = 4;
    m1.memory[5] = 5;
    /* end test program */

    m1.mem_max=127;
    world_add_bot(&w, &m1, &mp1);
    while(1) {
        world_tick(&w);
    }
}
