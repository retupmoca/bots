#ifndef BOTS_STRUCT_H
#define BOTS_STRUCT_H

#include <stdint.h>

typedef struct _bots_world bots_world;

typedef struct {
    /* cpu */
    uint16_t registers[12];
    uint8_t memory[65536];
    uint16_t user_mem_max;

    uint8_t fetch_flag;
    uint16_t fetch_pc;
    uint16_t fetched_pc;
    uint32_t fetched_instruction;

    uint8_t decode_flag;
    uint8_t decoded_pc;
    uint8_t decoded_opcode;
    uint8_t decoded_flags;
    uint8_t decoded_ra;
    uint8_t decoded_rb;
    uint16_t decoded_imm;

    uint8_t execute_cycle;

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

    uint16_t _req_steering;
    int16_t _req_throttle;
    uint16_t _req_turret_steering;
    uint8_t _req_turret_keepshift;
    uint16_t _req_scanner_steering;
    uint8_t _req_scanner_keepshift;
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
#define BOTS_EVENT_SCAN 3
#define BOTS_EVENT_HIT 4

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
