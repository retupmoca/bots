#pragma once

#include <stdio.h>
#include <bots/struct.hpp>

namespace bots {
    class World;
   
    char add_bot(World* g, char* memory, int size);
    char add_bot_from_handle(World* g, FILE* file);
    char add_bot_from_file(World* g, char* filename);
}
