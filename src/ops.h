#ifndef OPS_H
#define OPS_H

#include "struct.h"

typedef struct {
    char size;
    char argcount;
    char arg_sizes[3];
    void (*execute)(machine*);
} opdata;

void op_rset(machine*);
void op_nop(machine*);
void op_ldi(machine*);
void op_add(machine*);
void op_out(machine*);
void op_in(machine*);

opdata oplist[7];

#endif
