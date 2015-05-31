#include <stdio.h>
#include <stdlib.h>

#include "bots.h"

#ifdef _WIN32
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#endif

int main(int argc, char *argv[]) {
    bots_game *g = bots_create_game();

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

    /* hack the bots into position
     * 
     * We only do this because bots_add_bot doesn't position the bots at all
     * right now. In the future, it'll position them with some standard
     * algorithm.
     */
    g->tanks[0]->x = 50;
    g->tanks[0]->y = 50;

    while(1) {
        bots_tick(g);
        printf("Bot 1 position: %i:%i\n", g->tanks[0]->x, g->tanks[0]->y);
        printf("Bot 2 position: %i:%i\n", g->tanks[1]->x, g->tanks[1]->y);
        sleep(1);
    }
    bots_free_game(g);
    return 0;
}
