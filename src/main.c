#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "machine.h"
#include "ops.h"

void main() {
    world w = { 0 };
    machine m1 = { 0 };
    bot_physics mp1 = { 0 };
    mp1.x = 25;
    mp1.y = 50;

    /* test program */
    /* out 0 0 */
    m1.memory[0] = 4;
    m1.memory[1] = 0;
    m1.memory[2] = 0;

    /* in r3 1 */
    m1.memory[3] = 5;
    m1.memory[4] = 3;
    m1.memory[5] = 1;

    /* in r4 2 */
    m1.memory[6] = 5;
    m1.memory[7] = 4;
    m1.memory[8] = 2;
    /* end test program */

    m1.mem_max=127;
    w.bots[0] = &m1;
    w.botdata[0] = &mp1;
    m1.world = &w;
    while(1) {
        machine_execute(&m1);
        machine_decode(&m1);
        machine_fetch(&m1);
    }
}
