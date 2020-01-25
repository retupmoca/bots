#ifndef BOTS_STRUCT_H
#define BOTS_STRUCT_H

namespace bots { class World; }

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

    uint8_t bot_id;
} bots_cpu;

typedef struct _bots_peripheral bots_peripheral;
struct _bots_peripheral {
    uint16_t mem_base;

    void *_data;

    void (*process_tick)(bots_peripheral *p, bots::World *w, uint8_t bot_id, uint8_t pre);
};

#ifdef __cplusplus
}
#endif

#endif
