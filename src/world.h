#ifndef WORLD_H
#define WORLD_H
#include "struct.h"

void world_tick(world*);
void world_add_bot(world*, machine*, bot_physics*);

#endif
