#ifndef MACHINE_H
#define MACHINE_H

#include "struct.h"

void machine_execute(machine*);
void machine_decode(machine*);
void machine_fetch(machine*);

#endif
