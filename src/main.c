#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "machine.h"
#include "world.h"
#include "ops.h"

#ifdef _WIN32
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#endif

int main(int argc, char *argv[]) {
    world w = { 0 };
    machine m1 = { 0 };
    bot_physics mp1 = { 0 };
    mp1.x = 25;
    mp1.y = 50;
    mp1.health = 100;

    if(argc == 2){
        FILE *file = fopen(argv[1], "r");
        if( file != 0 ){
            int i = 0;
            char x;
            while(i < 128 && (x = fgetc(file)) != EOF){
                m1.memory[i++] = x;
            }
            fclose(file);
        }
    } else {
        printf("Please provide a single robot\n");
    }

    m1.mem_max=127;
    world_add_bot(&w, &m1, &mp1);
    while(1) {
        world_tick(&w);
        printf("Bot position: %i:%i\n", mp1.x, mp1.y);
        sleep(1);
    }
    return 0;
}
