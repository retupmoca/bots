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

void op_sub_ir(machine* m) {
    m->registers[m->args[0]] = m->args[1] - m->registers[m->args[2]];
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
  { 5, 3, {1, 2, 1}, &op_sub_ir },

  { 4, 3, {1, 1, 1}, &op_mul_rr },
  { 5, 3, {1, 1, 2}, &op_mul_ri },

  { 4, 3, {1, 1, 1}, &op_div_rr },
  { 5, 3, {1, 1, 2}, &op_div_ri },
  { 5, 3, {1, 2, 1}, &op_div_ir },

  { 4, 3, {1, 1, 1}, &op_mod_rr },
  { 5, 3, {1, 1, 2}, &op_mod_ri },
  { 5, 3, {1, 2, 1}, &op_mod_ir },

  { 3, 2, {1, 1}, &op_neg },

  { 4, 3, {1, 1, 1}, &op_or_rr },
  { 5, 3, {1, 1, 2}, &op_or_ri },

  { 4, 3, {1, 1, 1}, &op_and_rr },
  { 5, 3, {1, 1, 2}, &op_and_ri },

  { 4, 3, {1, 1, 1}, &op_xor_rr },
  { 5, 3, {1, 1, 2}, &op_xor_ri },

  { 4, 3, {1, 1, 1}, &op_shl_rr },
  { 5, 3, {1, 1, 2}, &op_shl_ri },
  { 4, 3, {1, 2, 1}, &op_shl_ir },

  { 4, 3, {1, 1, 1}, &op_shr_rr },
  { 5, 3, {1, 1, 2}, &op_shr_ri },
  { 5, 3, {1, 2, 1}, &op_shr_ir },

  { 3, 2, {1, 1}, &op_not },

  { 2, 1, {1}, &op_push_r },
  { 3, 1, {2}, &op_push_i },

  { 2, 1, {1}, &op_pop },

  { 3, 2, {1, 1}, &op_put_rr },
  { 4, 2, {1, 2}, &op_put_ri },
  { 4, 2, {2, 1}, &op_put_ir },
  { 5, 2, {2, 2}, &op_put_ii },

  { 3, 2, {1, 1}, &op_get_r },
  { 4, 2, {1, 2}, &op_get_i },

  { 3, 2, {1, 1}, &op_out_rr },
  { 4, 2, {1, 2}, &op_out_ri },
  { 4, 2, {2, 1}, &op_out_ir },
  { 5, 2, {2, 2}, &op_out_ii },

  { 3, 2, {1, 1}, &op_in_r },
  { 4, 2, {1, 2}, &op_in_i },

  { 2, 1, {1}, &op_int_r },
  { 3, 1, {2}, &op_int_i },

  { 3, 2, {1, 1}, &op_cmp_rr },
  { 4, 2, {1, 2}, &op_cmp_ri },
  { 4, 2, {2, 1}, &op_cmp_ir },

  { 2, 1, {1}, &op_jmp_r },
  { 3, 1, {2}, &op_jmp_i },

  { 2, 1, {1}, &op_jls_r },
  { 3, 1, {2}, &op_jls_i },

  { 2, 1, {1}, &op_jgr_r },
  { 3, 1, {2}, &op_jgr_i },

  { 2, 1, {1}, &op_jne_r },
  { 3, 1, {2}, &op_jne_i },

  { 2, 1, {1}, &op_jeq_r },
  { 3, 1, {2}, &op_jeq_i },

  { 2, 1, {1}, &op_jge_r },
  { 3, 1, {2}, &op_jge_i },

  { 2, 1, {1}, &op_jle_r },
  { 3, 1, {2}, &op_jle_i },

  { 2, 1, {1}, &op_jz_r },
  { 3, 1, {2}, &op_jz_i },

  { 2, 1, {1}, &op_jnz_r },
  { 3, 1, {2}, &op_jnz_i },
};
