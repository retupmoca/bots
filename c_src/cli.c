#include <unistd.h>
#include <stdio.h>

#include "../bots.h"

int main() {
    bots_world_config config = bots_default_world_config();
    bots_world * world = bots_world_new(&config);

    bots_world_add_bot(world, "testbot.bc");
    bots_world_add_bot(world, "testbot.bc");

    bots_world_place_bots(world);

    while (1) {
        bots_tick(world);
        const bots_tank * tank = bots_get_tank(world, 0);
        printf("Bot 1 position: %d:%d\n", tank->x, tank->y);
        tank = bots_get_tank(world, 1);
        printf("Bot 2 position: %d:%d\n", tank->x, tank->y);
        sleep(1);
    }

    return 0;
}
