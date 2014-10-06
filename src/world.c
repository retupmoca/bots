#include <math.h>

#include "world.h"
#include "machine.h"

void world_tick(world* w){
    int i = 0;
    /* physics */
    for(i=0; i < w->botcount; i++){
        /* turn, etc */
        w->botdata[i]->heading = w->bots[i]->ports[3];
        w->botdata[i]->speed = w->bots[i]->ports[4];

        /* drive! */
        double rangle = w->botdata[i]->heading * M_PI / 128;
        int dist = w->botdata[i]->speed;
        int dx = floor(0.5 + (dist * cos(rangle)));
        int dy = floor(0.5 + (dist * sin(rangle)));
        w->botdata[i]->x += dx;
        w->botdata[i]->y += dy;
    }

    /* execute */
    for(i=0; i < w->botcount; i++){
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
