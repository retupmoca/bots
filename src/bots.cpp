#include <bots/world.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bots/bots.hpp>
#include <bots/peripherals.hpp>

namespace bots {
    char add_bot_from_file(World* g, char* filename) {
        FILE* f = fopen(filename, "rb");
        char ret = add_bot_from_handle(g, f);
        if(f)
            fclose(f);

        return ret;
    }
    char add_bot_from_handle(World* g, FILE* file) {
        if(!file){
            return 0;
        }

        char buf[65536];
        int i = 0;
        i = fread(buf, 1, 65536, file);

        return add_bot(g, buf, i);
    }
    char add_bot(World* g, char* memory, int size) {
        bots_cpu* cpu = (bots_cpu*)calloc(1, sizeof(bots_cpu));
        bots_tank* tank = (bots_tank*)calloc(1, sizeof(bots_tank));

        tank->health = 100;
        cpu->user_mem_max = 0xefff;
        cpu->registers[10] = 0xefff;
        memcpy(cpu->memory, memory, size);

        tank->peripherals = (bots_peripheral*)calloc(5, sizeof(bots_peripheral));
        tank->peripherals[0].mem_base = 0xfef0;
        tank->peripherals[0].process_tick = &bots_peripheral_reset;
        tank->peripherals[1].mem_base = 0xfee0;
        tank->peripherals[1].process_tick = &bots_peripheral_radar;
        tank->peripherals[2].mem_base = 0xfed0;
        tank->peripherals[2].process_tick = &bots_peripheral_turret;
        tank->peripherals[3].mem_base = 0xfec0;
        tank->peripherals[3].process_tick = &bots_peripheral_hull;

        g->add_bot(cpu, tank);

        return 1;
    }
}
