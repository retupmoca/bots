#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "bots.h"
#include "machine.h"
#include "world.h"
#include "ops.h"

bots_game* bots_create_game() {
    bots_game* g = (bots_game*)malloc(sizeof(bots_game));

    g->num_tanks = 0;
    g->_world = (world*)calloc(1, sizeof(world));

    return g;
}

void bots_free_game(bots_game* g){
    int i;
    for(i = 0; i < g->num_tanks; i++) {
        free(g->tanks[i]);
        free(g->cpus[i]);
    }
    free(g->_world);
    free(g);
}

char bots_add_bot_from_file(bots_game* g, char* filename) {
    FILE* f = fopen(filename, "r");
    char ret = bots_add_bot_from_handle(g, f);
    fclose(f);

    return ret;
}
char bots_add_bot_from_handle(bots_game* g, FILE* file) {
    if(!file){
        return 0;
    }

    char buf[128];
    int i = 0;
    char x;
    while(i < 128 && (x = fgetc(file)) != EOF){
        buf[i++] = x;
    }

    return bots_add_bot(g, buf, i);
}
char bots_add_bot(bots_game* g, char* memory, int size) {
    machine* cpu = (machine*)calloc(1, sizeof(machine));
    bot_physics* tank = (bot_physics*)calloc(1, sizeof(bot_physics));

    tank->health = 100;
    cpu->mem_max = 127;
    memcpy(cpu->memory, memory, size);

    world_add_bot(g->_world, cpu, tank);

    g->tanks[g->num_tanks] = tank;
    g->cpus[g->num_tanks++] = cpu;

    return 1;
}

void bots_tick(bots_game* g) {
    world_tick(g->_world);
}
