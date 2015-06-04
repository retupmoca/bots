#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bots/bots.h>

bots_world* bots_create_world() {
    bots_world* g = (bots_world*)calloc(1, sizeof(bots_world));

    return g;
}

void bots_free_world(bots_world* g){
    int i;
    for(i = 0; i < g->num_tanks; i++) {
        free(g->tanks[i]);
        free(g->cpus[i]);
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
    cpu->mem_max = 65535;
    cpu->registers[1] = 65534;
    memcpy(cpu->memory, memory, size);

    bots_world_add_bot(g, cpu, tank);

    return 1;
}

void bots_tick(bots_world* g) {
    bots_world_tick(g);
}
