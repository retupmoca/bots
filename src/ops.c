#include "ops.h"

void int_reqpos(machine* m) {
    m->ports[1] = m->world->botdata[m->machine_id]->x;
    m->ports[2] = m->world->botdata[m->machine_id]->y;
}

typedef void (*action)(machine*);

action int_actions[16] = {
    &int_reqpos
};

void op_rset(machine* m) {
    m->fetch_pc = 0;
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_nop(machine* m) {
    /* nop nop nop */
}

void op_ldi(machine* m) {
    m->registers[m->args[0]] = m->args[1];
}

void op_add(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->registers[m->args[2]];
}

void op_out(machine* m) {
    m->ports[m->args[0]] = m->args[1];
}

void op_in(machine* m) {
    m->registers[m->args[0]] = m->ports[m->args[1]];
}

void op_eint(machine* m) {
    if(int_actions[m->args[0]]){
        (*int_actions[m->args[0]])(m);
    }
}

opdata oplist[7] = {
  { 1, 0, {}, &op_rset },
  { 1, 0, {}, &op_nop },
  { 3, 2, {1, 1}, &op_ldi },
  { 4, 3, {1, 1, 1}, &op_add },
  { 3, 2, {1, 1}, &op_out },
  { 3, 2, {1, 1}, &op_in },
  { 2, 1, {1}, &op_eint }
};
