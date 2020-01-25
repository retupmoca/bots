#ifndef BOTS_STRUCT_H
#define BOTS_STRUCT_H

namespace bots { class World; }

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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
