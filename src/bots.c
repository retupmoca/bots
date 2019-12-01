#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bots/bots.h>
#include <bots/world.h>
#include <bots/peripherals.h>

bots_world* bots_create_world() {
    bots_world* g = (bots_world*)calloc(1, sizeof(bots_world));

    g->c_cpus_per_tick = 2;

    return g;
}

void bots_free_world(bots_world* g){
    int i;
    for(i = 0; i < g->num_tanks; i++) {
        free(g->tanks[i]);
        free(g->cpus[i]);
    }

    if(g->_tick_events){
        if(g->_tick_events->events) {
            free(g->_tick_events->events);
        }
        free(g->_tick_events);
    }
    free(g);
}

char bots_add_bot_from_file(bots_world* g, char* filename) {
    FILE* f = fopen(filename, "rb");
    char ret = bots_add_bot_from_handle(g, f);
    if(f)
        fclose(f);

    return ret;
}
char bots_add_bot_from_handle(bots_world* g, FILE* file) {
    if(!file){
        return 0;
    }

    char buf[65536];
    int i = 0;
    i = fread(buf, 1, 65536, file);

    return bots_add_bot(g, buf, i);
}
char bots_add_bot(bots_world* g, char* memory, int size) {
    bots_cpu* cpu = (bots_cpu*)calloc(1, sizeof(bots_cpu));
    bots_tank* tank = (bots_tank*)calloc(1, sizeof(bots_tank));

    tank->health = 100;
    cpu->user_mem_max = 0xefff;
    cpu->registers[10] = 0xefff;
    memcpy(cpu->memory, memory, size);

    tank->peripherals = calloc(5, sizeof(bots_peripheral));
    tank->peripherals[0].mem_base = 0xfef0;
    tank->peripherals[0].process_tick = &bots_peripheral_reset;
    tank->peripherals[1].mem_base = 0xfee0;
    tank->peripherals[1].process_tick = &bots_peripheral_radar;
    tank->peripherals[2].mem_base = 0xfed0;
    tank->peripherals[2].process_tick = &bots_peripheral_turret;
    tank->peripherals[3].mem_base = 0xfec0;
    tank->peripherals[3].process_tick = &bots_peripheral_hull;

    bots_world_add_bot(g, cpu, tank);

    return 1;
}

bots_events* bots_tick(bots_world* g) {
    return bots_world_tick(g);
}
