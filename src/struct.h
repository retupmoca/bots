#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>

typedef struct s_world world;

typedef struct {
    /* cpu */
    uint16_t registers[8];
    uint16_t pc;
    uint16_t mem_max;
    uint8_t memory[65536];
    
    uint8_t execute_ready;
    uint8_t op;
    uint16_t args[4];
    
    uint8_t decode_ready;
    uint16_t decode_pc;
    uint8_t decode_bytes[8];

    uint16_t fetch_pc;
    /**/

    uint8_t ports[24];

    world* world;
    uint8_t machine_id;
} machine;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t heading;
    int32_t speed;
    
    uint32_t turret_offset;
    uint32_t scanner_offset;
    
    uint8_t health;
} bot_physics;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t heading;
} shot;

struct s_world {
    uint8_t botcount;
    machine* bots[16];
    bot_physics* botdata[16];
    shot* shots[16*1024]; /* we've got the memory... */
};

#endif
