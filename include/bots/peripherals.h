#ifndef BOTS_PERIPHERALS_H
#define BOTS_PERIPHERALS_H
#include <bots/struct.h>

void bots_peripheral_reset(bots_peripheral *p, bots_world *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_radar(bots_peripheral *p, bots_world *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_turret(bots_peripheral *p, bots_world *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_hull(bots_peripheral *p, bots_world *w, uint8_t bot_id, uint8_t pre);

#endif
