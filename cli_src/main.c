#include <stdio.h>
#include <stdlib.h>

#include <bots/bots.h>

#ifdef _WIN32
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#else
# include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    bots_world *g = bots_create_world();

    if(argc == 3) {
        if(   !bots_add_bot_from_file(g, argv[1])
           || !bots_add_bot_from_file(g, argv[2])) {
            printf("Unable to load bots\n");
            return 1;
        }
    }
    else {
        printf("Please provide two robots\n");
    }

    bots_spawn(g);

    g->c_cpus_per_tick = 1;

    while(1) {
        bots_tick(g);
        printf("Bot 1 heading: %i\n", g->tanks[0]->heading);
        printf("Bot 1 position: %i:%i\n", g->tanks[0]->x, g->tanks[0]->y);
        printf("Bot 2 position: %i:%i\n", g->tanks[1]->x, g->tanks[1]->y);
        sleep(1);
    }
    bots_free_world(g);
    return 0;
}
