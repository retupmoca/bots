#ifndef BOTS_STRUCT_H
#define BOTS_STRUCT_H

#include <stdint.h>

typedef struct _bots_world bots_world;

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

    uint8_t ports[28];

    bots_world* world;
    uint8_t bot_id;
} bots_cpu;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t heading;
    int32_t speed;
    
    uint32_t turret_offset;
    uint32_t scanner_offset;
    
    uint8_t health;
} bots_tank;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t heading;

    uint8_t bot_id;
    long long id;
} bots_shot;

#define BOTS_EVENT_NOTHING 0
#define BOTS_EVENT_FIRE 1
#define BOTS_EVENT_DEATH 2

typedef struct {
    uint8_t event_type;

    uint8_t bot_id;
} bots_event;

typedef struct {
    bots_event *events;
    int event_count;

    int _size;
} bots_events;

struct _bots_world {
    uint8_t num_tanks;
    bots_cpu* cpus[16];
    bots_tank* tanks[16];
    bots_shot* shots[16*1024]; /* we've got the memory... */
    long long next_shot_id;
    
    bots_events* _tick_events;
};

#endif
