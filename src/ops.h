#ifndef OPS_H
#define OPS_H

#include <stdint.h>

#include "struct.h"

#define OPCOUNT 62

typedef struct {
    uint8_t size;
    uint8_t argcount;
    uint8_t arg_sizes[3];
    void (*execute)(machine*);
} opdata;

opdata oplist[OPCOUNT];

#endif
