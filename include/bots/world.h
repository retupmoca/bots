#ifndef BOTS_WORLD_H
#define BOTS_WORLD_H
#include <bots/struct.h>

#ifndef DLL_EXPORT
# define DLL_EXPORT
# ifdef _WIN32
#  ifdef BOTS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# endif
#endif

DLL_EXPORT void bots_world_tick(bots_world*);
DLL_EXPORT void bots_world_add_bot(bots_world*, bots_cpu*, bots_tank*);

#endif
