#define _USE_MATH_DEFINES
#include <math.h>

#include "world.h"
#include "machine.h"

void world_tick(world* w){
    int i = 0;
    /* physics */
    for(i=0; i < w->botcount; i++){
        /* turn, etc */
        uint16_t throttle;
        uint16_t steering;
        
        throttle = w->bots[i]->ports[0] << 8;
        throttle = throttle | w->bots[i]->ports[1];
        
        steering = w->bots[i]->ports[2] << 8;
        steering = steering | w->bots[i]->ports[3];
        
        w->botdata[i]->heading = (w->botdata[i]->heading + steering) % 256;
        w->bots[i]->ports[2] = 0;
        w->bots[i]->ports[3] = 0;
        w->botdata[i]->speed = throttle;

        /* drive! */
        double rangle = (w->botdata[i]->heading-64) * M_PI / 128;
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
