#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/ops.h>

void op_nop(bots_cpu* m) {
    /* nop nop nop */
}

void op_mov_r(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]];
}

void op_mov_i(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1];
}

void op_add_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->registers[m->args[2]];
}

void op_add_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->args[2];
}

void op_sub_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] - m->registers[m->args[2]];
}

void op_sub_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] - m->args[2];
}

void op_sub_ir(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1] - m->registers[m->args[2]];
}

void op_mul_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] * m->registers[m->args[2]];
}

void op_mul_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] * m->args[2];
}

void op_div_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] / m->registers[m->args[2]];
}

void op_div_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] / m->args[2];
}

void op_div_ir(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1] / m->registers[m->args[2]];
}

void op_mod_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] % m->registers[m->args[2]];
}

void op_mod_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] % m->args[2];
}

void op_mod_ir(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1] % m->registers[m->args[2]];
}

void op_neg(bots_cpu* m) {
    m->registers[m->args[0]] = 0 - m->registers[m->args[1]];
}

void op_or_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] | m->registers[m->args[2]];
}

void op_or_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] | m->args[2];
}

void op_and_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] & m->registers[m->args[2]];
}

void op_and_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] & m->args[2];
}

void op_xor_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] ^ m->registers[m->args[2]];
}

void op_xor_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] ^ m->args[2];
}

void op_shl_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] << m->registers[m->args[2]];
}

void op_shl_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] << m->args[2];
}

void op_shl_ir(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1] << m->registers[m->args[2]];
}

void op_shr_rr(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] >> m->registers[m->args[2]];
}

void op_shr_ri(bots_cpu* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] >> m->args[2];
}

void op_shr_ir(bots_cpu* m) {
    m->registers[m->args[0]] = m->args[1] >> m->registers[m->args[2]];
}

void op_not(bots_cpu* m) {
    m->registers[m->args[0]] = ~ m->registers[m->args[1]];
}

/* push/pop use register 1 as the stack pointer */
/* pointer is assumed to be at the end of usable memory and grows down */
void op_push_r(bots_cpu* m) {
    m->registers[10] -= 2;
    m->memory[m->registers[10]] = m->args[0] >> 8;
    m->memory[m->registers[10] + 1] = m->args[0] & 0xff;
}
void op_push_i(bots_cpu* m) {
    m->registers[10] -= 2;
    m->memory[m->registers[10]] = m->args[0] >> 8;
    m->memory[m->registers[10] + 1] = m->args[0] & 0xff;
}
void op_pop(bots_cpu* m) {
    m->registers[m->args[0]] = m->memory[m->registers[10]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[10] + 1];
    m->registers[10] += 2;
}
/**/

void op_sw_rrr(bots_cpu* m) {
    uint16_t address = m->registers[m->args[0]] + m->registers[m->args[1]];
    uint16_t value = m->registers[m->args[2]];
    m->memory[address] = value >> 8;
    m->memory[address + 1] = value & 0xff;
}

void op_sb_rrr(bots_cpu* m) {
    uint16_t address = m->registers[m->args[0]] + m->registers[m->args[1]];
    uint16_t value = m->registers[m->args[2]];
    m->memory[address] = m->registers[m->args[2]] & 0xff;
}

void op_sw_rir(bots_cpu* m) {
    uint16_t address = m->registers[m->args[0]] + m->args[1];
    uint16_t value = m->registers[m->args[2]];
    m->memory[address] = value >> 8;
    m->memory[address + 1] = value & 0xff;
}

void op_sb_rir(bots_cpu* m) {
    uint16_t address = m->registers[m->args[0]] + m->args[1];
    uint16_t value = m->registers[m->args[2]];
    m->memory[address] = m->registers[m->args[2]] & 0xff;
}

void op_lw_rrr(bots_cpu* m) {
    uint16_t address = m->registers[m->args[1]] + m->registers[m->args[2]];
    m->registers[m->args[0]] = m->memory[address] << 8;
    m->registers[m->args[0]] |= m->memory[address + 1];
}

void op_lb_rrr(bots_cpu* m) {
    uint16_t address = m->registers[m->args[1]] + m->registers[m->args[2]];
    m->registers[m->args[0]] = m->memory[address];
}

void op_lw_rri(bots_cpu* m) {
    uint16_t address = m->registers[m->args[1]] + m->args[2];
    m->registers[m->args[0]] = m->memory[address] << 8;
    m->registers[m->args[0]] |= m->memory[address + 1];
}

void op_lb_rri(bots_cpu* m) {
    uint16_t address = m->registers[m->args[1]] + m->args[2];
    m->registers[m->args[0]] = m->memory[address];
}

void op_out_rr(bots_cpu* m) {
    m->ports[m->registers[m->args[0]]] = m->registers[m->args[1]] >> 8;
    m->ports[m->registers[m->args[0]] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_out_ri(bots_cpu* m) {
    m->ports[m->registers[m->args[0]]] = m->args[1] >> 8;
    m->ports[m->registers[m->args[0]] + 1] = m->args[1] & 0xff;
}

void op_out_ir(bots_cpu* m) {
    m->ports[m->args[0]] = m->registers[m->args[1]] >> 8;
    m->ports[m->args[0] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_out_ii(bots_cpu* m) {
    m->ports[m->args[0]] = m->args[1] >> 8;
    m->ports[m->args[0] + 1] = m->args[1] & 0xff;
}

void op_in_r(bots_cpu* m) {
    m->registers[m->args[0]] = m->ports[m->registers[m->args[1]]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->ports[m->registers[m->args[1]] + 1];
}

void op_in_i(bots_cpu* m) {
    m->registers[m->args[0]] = m->ports[m->args[1]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->ports[m->args[1] + 1];
}

void op_int_r(bots_cpu* m) {
}

void op_int_i(bots_cpu* m) {
}

/* cmp and all jumps use register 0 for flags */
static void set_compare_flags(bots_cpu* m, uint16_t val_a, uint16_t val_b) {
    m->registers[11] = m->registers[11] & 0x0fff;
    
    if(val_a < val_b)
        m->registers[11] = m->registers[11] | 0x8000;
    if(val_a > val_b)
        m->registers[11] = m->registers[11] | 0x4000;
    if(val_a == val_b)
        m->registers[11] = m->registers[11] | 0x2000;
    if(val_a == 0 && val_b == 0)
        m->registers[11] = m->registers[11] | 0x1000;
}

void op_cmp_rr(bots_cpu* m) {
    set_compare_flags(m, m->registers[m->args[0]], m->registers[m->args[1]]);
}

void op_cmp_ri(bots_cpu* m) {
    set_compare_flags(m, m->registers[m->args[0]], m->args[1]);
}

void op_cmp_ir(bots_cpu* m) {
    set_compare_flags(m, m->args[0], m->registers[m->args[1]]);
}

void op_jmp_r(bots_cpu* m) {
    m->fetch_pc = m->registers[m->args[0]];
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_jmp_i(bots_cpu* m) {
    /* the cpu fetcher already followed the jump */
    /*m->fetch_pc = m->args[0];
    m->decode_ready = 0;
    m->execute_ready = 0;*/
}

void op_jls_r(bots_cpu* m) {
    if(m->registers[11] & 0x8000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jls_i(bots_cpu* m) {
    if(m->registers[11] & 0x8000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_r(bots_cpu* m) {
    if(m->registers[11] & 0x4000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_i(bots_cpu* m) {
    if(m->registers[11] & 0x4000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_r(bots_cpu* m) {
    if(!(m->registers[11] & 0x2000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_i(bots_cpu* m) {
    if(!(m->registers[11] & 0x2000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_r(bots_cpu* m) {
    if(m->registers[11] & 0x2000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_i(bots_cpu* m) {
    if(m->registers[11] & 0x2000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_r(bots_cpu* m) {
    if(m->registers[11] & 0x6000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_i(bots_cpu* m) {
    if(m->registers[11] & 0x6000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_r(bots_cpu* m) {
    if(m->registers[11] & 0xa000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_i(bots_cpu* m) {
    if(m->registers[11] & 0xa000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_r(bots_cpu* m) {
    if(m->registers[11] & 0x1000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_i(bots_cpu* m) {
    if(m->registers[11] & 0x1000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_r(bots_cpu* m) {
    if(!(m->registers[11] & 0x1000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_i(bots_cpu* m) {
    if(!(m->registers[11] & 0x1000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_call_r(bots_cpu* m) {
    m->registers[10] -= 2;
    m->memory[m->registers[10]] = m->decode_pc >> 8;
    m->memory[m->registers[10] + 1] = m->decode_pc & 0xff;

    m->fetch_pc = m->registers[m->args[0]];
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_call_i(bots_cpu* m) {
    m->registers[10] -= 2;
    uint16_t pc = m->pc;
    pc += 3;
    m->memory[m->registers[10]] = pc >> 8;
    m->memory[m->registers[10] + 1] = pc & 0xff;

    /* the cpu fetcher already followed the jump */
    /*m->fetch_pc = m->args[0];
    m->decode_ready = 0;
    m->execute_ready = 0;*/
}

void op_ret(bots_cpu* m) {
    uint16_t pc = 0;
    pc = m->memory[m->registers[10]] << 8;
    pc = pc | m->memory[m->registers[10] + 1];
    m->registers[10] += 2;

    m->fetch_pc = pc;
    m->decode_ready = 0;
    m->execute_ready = 0;
}

bots_cpu_opdata bots_cpu_oplist[BOTS_CPU_OPCOUNT] = {
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

  { 4, 3, {1, 1, 1}, &op_sw_rrr },
  { 4, 3, {1, 1, 1}, &op_sb_rrr },
  { 5, 3, {1, 2, 1}, &op_sw_rir },
  { 5, 3, {1, 2, 1}, &op_sb_rir },

  { 4, 3, {1, 1, 1}, &op_lw_rrr },
  { 4, 3, {1, 1, 1}, &op_lb_rrr },
  { 5, 3, {1, 1, 2}, &op_lw_rri },
  { 5, 3, {1, 1, 2}, &op_lb_rri },

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

  { 2, 1, {1}, &op_call_r },
  { 3, 1, {2}, &op_call_i },
  { 1, 0, {0}, &op_ret },
};
