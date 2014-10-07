#include "ops.h"

void op_rset(machine* m) {
    m->fetch_pc = 0;
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void int_reqpos(machine* m) {
    m->ports[1] = m->world->botdata[m->machine_id]->x;
    m->ports[2] = m->world->botdata[m->machine_id]->y;
}

typedef void (*action)(machine*);

action int_actions[16] = {
    &int_reqpos
};

void op_nop(machine* m) {
    /* nop nop nop */
}

void op_mov_r(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]];
}

void op_mov_i(machine* m) {
    m->registers[m->args[0]] = m->args[1];
}

void op_add_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->registers[m->args[2]];
}

void op_add_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->args[2];
}

void op_sub_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] - m->registers[m->args[2]];
}

void op_sub_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] - m->args[2];
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

opdata oplist[OPCOUNT] = {
  { 1, 0, {}, &op_nop },
  { 3, 2, {1, 1}, &op_mov_r },
  { 4, 2, {1, 2}, &op_mov_i },
  { 4, 3, {1, 1, 1}, &op_add_rr },
  { 5, 3, {1, 1, 2}, &op_add_ri },
  { 4, 3, {1, 1, 1}, &op_sub_rr },
  { 5, 3, {1, 1, 2}, &op_sub_ri },
};
