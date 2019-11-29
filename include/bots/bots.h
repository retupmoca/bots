#ifndef BOTS_BOTS_H
#define BOTS_BOTS_H
#include <stdio.h>
#include <bots/struct.h>

#ifndef DLL_EXPORT
# ifdef _WIN32
#  ifdef BOTS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# else
#  define DLL_EXPORT
# endif
#endif

DLL_EXPORT bots_world* bots_create_world();
DLL_EXPORT void bots_free_world(bots_world* g);

DLL_EXPORT char bots_add_bot(bots_world* g, char* memory, int size);
DLL_EXPORT char bots_add_bot_from_handle(bots_world* g, FILE* file);
DLL_EXPORT char bots_add_bot_from_file(bots_world* g, char* filename);

DLL_EXPORT bots_events* bots_tick(bots_world* g);
#endif
