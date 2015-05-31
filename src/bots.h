#ifndef BOTS_H
#define BOTS_H
#include<stdio.h>
#include "struct.h"

#define DLL_EXPORT
#ifdef _WIN32
#ifdef BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#endif

typedef struct {
    int num_tanks;
    bot_physics* tanks[16];
    machine* cpus[16];

    world* _world;
} bots_game;

DLL_EXPORT bots_game* bots_create_game();
DLL_EXPORT void bots_free_game(bots_game* g);

DLL_EXPORT char bots_add_bot(bots_game* g, char* memory, int size);
DLL_EXPORT char bots_add_bot_from_handle(bots_game* g, FILE* file);
DLL_EXPORT char bots_add_bot_from_file(bots_game* g, char* filename);

DLL_EXPORT void bots_tick(bots_game* g);
#endif
