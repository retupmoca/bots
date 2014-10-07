#ifndef OPS_H
#define OPS_H

#include <stdint.h>

#include "struct.h"

#define OPCOUNT 7

typedef struct {
    uint8_t size;
    uint8_t argcount;
    uint8_t arg_sizes[3];
    void (*execute)(machine*);
} opdata;

void op_rset(machine*);
void op_nop(machine*);
void op_ldi(machine*);
void op_add(machine*);
void op_out(machine*);
void op_in(machine*);

opdata oplist[OPCOUNT];

#endif
