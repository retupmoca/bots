#pragma once

#include <bots/struct.hpp>

namespace bots { class World; }

void bots_peripheral_reset(bots_peripheral *p, bots::World *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_radar(bots_peripheral *p, bots::World *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_turret(bots_peripheral *p, bots::World *w, uint8_t bot_id, uint8_t pre);
void bots_peripheral_hull(bots_peripheral *p, bots::World *w, uint8_t bot_id, uint8_t pre);
