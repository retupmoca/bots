#include "world.h"
#include "machine.h"

void world_tick(world* w){
    /* physics */
    /* TODO */

    /* execute */
    int i = 0;
    for(; i < w->botcount; i++){
        /* we do these "backwords" in order to simulate a 3-stage pipeline */
        machine_execute(w->bots[i]);
        machine_decode(w->bots[i]);
        machine_fetch(w->bots[i]);
    }
}

void world_add_bot(world* w, machine* m, bot_physics* p) {
    if(w->botcount == 16){
        return;
    }
    m->world = w;
    m->machine_id = w->botcount;
    w->bots[w->botcount] = m;
    w->botdata[w->botcount] = p;
    w->botcount++;
}
