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

void op_mul_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] * m->registers[m->args[2]];
}

void op_mul_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] * m->args[2];
}

void op_div_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] / m->registers[m->args[2]];
}

void op_div_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] / m->args[2];
}

void op_div_ir(machine* m) {
    m->registers[m->args[0]] = m->args[1] / m->registers[m->args[2]];
}

void op_mod_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] % m->registers[m->args[2]];
}

void op_mod_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] % m->args[2];
}

void op_mod_ir(machine* m) {
    m->registers[m->args[0]] = m->args[1] % m->registers[m->args[2]];
}

void op_neg(machine* m) {
    m->registers[m->args[0]] = 0 - m->registers[m->args[1]];
}

void op_or_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] | m->registers[m->args[2]];
}

void op_or_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] | m->args[2];
}

void op_and_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] & m->registers[m->args[2]];
}

void op_and_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] & m->args[2];
}

void op_xor_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] ^ m->registers[m->args[2]];
}

void op_xor_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] ^ m->args[2];
}

void op_shl_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] << m->registers[m->args[2]];
}

void op_shl_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] << m->args[2];
}

void op_shl_ir(machine* m) {
    m->registers[m->args[0]] = m->args[1] << m->registers[m->args[2]];
}

void op_shr_rr(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] >> m->registers[m->args[2]];
}

void op_shr_ri(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] >> m->args[2];
}

void op_shr_ir(machine* m) {
    m->registers[m->args[0]] = m->args[1] >> m->registers[m->args[2]];
}

void op_not(machine* m) {
    m->registers[m->args[0]] = ~ m->registers[m->args[1]];
}

/* push/pop use register 1 as the stack pointer */
/* pointer is assumed to be at the end of usable memory and grows down */
void op_push_r(machine* m) {
    m->registers[1] -= 2;
    m->memory[m->registers[1]] = m->args[0] >> 8;
    m->memory[m->registers[1] + 1] = m->args[0] & 0xff;
}
void op_push_i(machine* m) {
    m->registers[1] -= 2;
    m->memory[m->registers[1]] = m->args[0] >> 8;
    m->memory[m->registers[1] + 1] = m->args[0] & 0xff;
}
void op_pop(machine* m) {
    m->registers[m->args[0]] = m->memory[m->registers[1]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[1] + 1];
    m->registers[1] += 2;
}
/**/

void op_put_rr(machine* m) {
    m->memory[m->registers[m->args[0]]] = m->registers[m->args[1]] >> 8;
    m->memory[m->registers[m->args[0]] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_put_ri(machine* m) {
    m->memory[m->registers[m->args[0]]] = m->args[1] >> 8;
    m->memory[m->registers[m->args[0]] + 1] = m->args[1] & 0xff;
}

void op_put_ir(machine* m) {
    m->memory[m->args[0]] = m->registers[m->args[1]] >> 8;
    m->memory[m->args[0] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_put_ii(machine* m) {
    m->memory[m->args[0]] = m->args[1] >> 8;
    m->memory[m->args[0] + 1] = m->args[1] & 0xff;
}

void op_get_r(machine* m) {
    m->registers[m->args[0]] = m->memory[m->registers[m->args[1]]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[m->args[1]] + 1];
}

void op_get_i(machine* m) {
    m->registers[m->args[0]] = m->memory[m->args[1]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->args[1] + 1];
}

void op_out_rr(machine* m) {
    m->ports[m->registers[m->args[0]]] = m->registers[m->args[1]] >> 8;
    m->ports[m->registers[m->args[0]] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_out_ri(machine* m) {
    m->ports[m->registers[m->args[0]]] = m->args[1] >> 8;
    m->ports[m->registers[m->args[0]] + 1] = m->args[1] & 0xff;
}

void op_out_ir(machine* m) {
    m->ports[m->args[0]] = m->registers[m->args[1]] >> 8;
    m->ports[m->args[0] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_out_ii(machine* m) {
    m->ports[m->args[0]] = m->args[1] >> 8;
    m->ports[m->args[0] + 1] = m->args[1] & 0xff;
}

void op_in_r(machine* m) {
    m->ports[m->args[0]] = m->memory[m->registers[m->args[1]]] << 8;
    m->ports[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[m->args[1]] + 1];
}

void op_in_i(machine* m) {
    m->ports[m->args[0]] = m->memory[m->args[1]] << 8;
    m->ports[m->args[0]] = m->registers[m->args[0]] | m->memory[m->args[1] + 1];
}

void op_int_r(machine* m) {
    (*int_actions[m->registers[m->args[0]]])(m);
}

void op_int_i(machine* m) {
    (*int_actions[m->args[0]])(m);
}

/* cmp and all jumps use register 0 for flags */
static void set_compare_flags(machine* m, uint16_t val_a, uint16_t val_b) {
    m->registers[0] = m->registers[0] & 0x0fff;
    
    if(val_a < val_b)
        m->registers[0] = m->registers[0] | 0x8000;
    if(val_a > val_b)
        m->registers[0] = m->registers[0] | 0x4000;
    if(val_a == val_b)
        m->registers[0] = m->registers[0] | 0x2000;
    if(val_a == 0 && val_b == 0)
        m->registers[0] = m->registers[0] | 0x1000;
}

void op_cmp_rr(machine* m) {
    set_compare_flags(m, m->registers[m->args[0]], m->registers[m->args[1]]);
}

void op_cmp_ri(machine* m) {
    set_compare_flags(m, m->registers[m->args[0]], m->args[1]);
}

void op_cmp_ir(machine* m) {
    set_compare_flags(m, m->args[0], m->registers[m->args[1]]);
}

void op_jmp_r(machine* m) {
    m->fetch_pc = m->registers[m->args[0]];
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_jmp_i(machine* m) {
    m->fetch_pc = m->args[0];
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_jls_r(machine* m) {
    if(m->registers[0] & 0x8000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jls_i(machine* m) {
    if(m->registers[0] & 0x8000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_r(machine* m) {
    if(m->registers[0] & 0x4000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_i(machine* m) {
    if(m->registers[0] & 0x4000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_r(machine* m) {
    if(!(m->registers[0] & 0x2000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_i(machine* m) {
    if(!(m->registers[0] & 0x2000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_r(machine* m) {
    if(m->registers[0] & 0x2000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_i(machine* m) {
    if(m->registers[0] & 0x2000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_r(machine* m) {
    if(m->registers[0] & 0x6000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_i(machine* m) {
    if(m->registers[0] & 0x6000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_r(machine* m) {
    if(m->registers[0] & 0xa000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_i(machine* m) {
    if(m->registers[0] & 0xa000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_r(machine* m) {
    if(m->registers[0] & 0x1000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_i(machine* m) {
    if(m->registers[0] & 0x1000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_r(machine* m) {
    if(!(m->registers[0] & 0x1000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_i(machine* m) {
    if(!(m->registers[0] & 0x1000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

/*void op_out(machine* m) {
    m->ports[m->args[0]] = m->args[1];
}

void op_in(machine* m) {
    m->registers[m->args[0]] = m->ports[m->args[1]];
}

void op_eint(machine* m) {
    if(int_actions[m->args[0]]){
        (*int_actions[m->args[0]])(m);
    }
}*/

opdata oplist[OPCOUNT] = {
  { 1, 0, {0}, &op_nop },

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
