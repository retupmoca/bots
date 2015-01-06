#ifndef WORLD_H
#define WORLD_H
#include "struct.h"

#define DLL_EXPORT
#ifdef _WIN32
#ifdef BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#endif
#endif

DLL_EXPORT void world_tick(world*);
DLL_EXPORT void world_add_bot(world*, machine*, bot_physics*);

#endif
