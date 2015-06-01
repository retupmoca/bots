#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/ops.h>

void int_rset(bots_cpu* m) {
    m->fetch_pc = 0;
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void int_destruct(bots_cpu* m) {
    m->world->tanks[m->bot_id]->health = 0;
}

void int_scan(bots_cpu* m) {
    /* get global heading of scanner */
    uint32_t heading = m->world->tanks[m->bot_id]->heading + m->world->tanks[m->bot_id]->turret_offset + m->world->tanks[m->bot_id]->scanner_offset;
    
    /* check angle and range of each bot against scan parameters */
    int radar_arc = m->ports[0x0f];
    int radar_range = m->ports[0x10] << 8;
    radar_range |= m->ports[0x11];
    
    uint8_t radar_left = (heading - radar_arc) % 256;
    uint8_t radar_right = (heading + radar_arc) % 256;
    
    int seen_index = 0;
    int seen_bots[256] = { -1 };
    int32_t seen_bot_range[256];
    int seen_bot_angle[256];
    
    int i;
    for(i=0; i<m->world->num_tanks; i++){
        if(i == m->bot_id)
            continue;
        
        int32_t x = m->world->tanks[i]->x - m->world->tanks[m->bot_id]->x;
        int32_t y = m->world->tanks[i]->y - m->world->tanks[m->bot_id]->y;
        
        uint8_t angle = (int)(atan2(y, x) * 128 / M_PI) % 256;
        int32_t range = (int)(sqrt(y * y + x * x));
        
        if(   range <= radar_range
           && (   (radar_left < radar_right && angle > radar_left && angle < radar_right)
               || (radar_left > radar_right && (angle > radar_left || angle < radar_right)))) {
            /* we can see bot i */
            seen_bot_range[seen_index] = range;
            seen_bot_angle[seen_index] = angle;
            seen_bots[seen_index++] = i;
        }
    }
    
    /* load closest seen bot into ports */
    uint16_t lowest_range = 0;
    m->ports[0x12] = 0;
    m->ports[0x13] = 0;
    m->ports[0x14] = 0;
    m->ports[0x15] = 0;
    for(i=0; seen_bots[i] > -1; i++) {
        if(seen_bot_range[i] < lowest_range || lowest_range == 0) {
            lowest_range = seen_bot_range[i];
            m->ports[0x12] = lowest_range >> 8;
            m->ports[0x13] = lowest_range & 0xff;
            
            /* TODO: give some kind of scanner offset instead of bearing */
            m->ports[0x14] = seen_bot_angle[i] >> 8;
            m->ports[0x15] = seen_bot_angle[i] & 0xff;
        }
    }
}

void int_fire(bots_cpu* m) {
    bots_shot* s = malloc(sizeof(bots_shot));
    
    /* get global heading of gun */
    s->heading = m->world->tanks[m->bot_id]->heading + m->world->tanks[m->bot_id]->turret_offset;
    
    /* move just far enough that we don't hit ourselves */
    s->x = m->world->tanks[m->bot_id]->x;
    s->y = m->world->tanks[m->bot_id]->y;
        
    double rangle = (s->heading-64) * M_PI / 128;
    int dist = 6;
    int dx = floor(0.5 + (dist * cos(rangle)));
    int dy = floor(0.5 + (dist * sin(rangle)));
    s->x += dx;
    s->y += dy;

    s->bot_id = m->bot_id;
    s->id = m->world->next_shot_id++;
    
    /* add shot to world */
    int i = 0;
    for(; m->world->shots[i]; i++)
        ;
    m->world->shots[i] = s;
}

typedef void (*action)(bots_cpu*);

action int_actions[4] = {
    &int_rset,
    &int_destruct,
    &int_scan,
    &int_fire
};

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
    m->registers[1] -= 2;
    m->memory[m->registers[1]] = m->args[0] >> 8;
    m->memory[m->registers[1] + 1] = m->args[0] & 0xff;
}
void op_push_i(bots_cpu* m) {
    m->registers[1] -= 2;
    m->memory[m->registers[1]] = m->args[0] >> 8;
    m->memory[m->registers[1] + 1] = m->args[0] & 0xff;
}
void op_pop(bots_cpu* m) {
    m->registers[m->args[0]] = m->memory[m->registers[1]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[1] + 1];
    m->registers[1] += 2;
}
/**/

void op_put_rr(bots_cpu* m) {
    m->memory[m->registers[m->args[0]]] = m->registers[m->args[1]] >> 8;
    m->memory[m->registers[m->args[0]] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_put_ri(bots_cpu* m) {
    m->memory[m->registers[m->args[0]]] = m->args[1] >> 8;
    m->memory[m->registers[m->args[0]] + 1] = m->args[1] & 0xff;
}

void op_put_ir(bots_cpu* m) {
    m->memory[m->args[0]] = m->registers[m->args[1]] >> 8;
    m->memory[m->args[0] + 1] = m->registers[m->args[1]] & 0xff;
}

void op_put_ii(bots_cpu* m) {
    m->memory[m->args[0]] = m->args[1] >> 8;
    m->memory[m->args[0] + 1] = m->args[1] & 0xff;
}

void op_get_r(bots_cpu* m) {
    m->registers[m->args[0]] = m->memory[m->registers[m->args[1]]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[m->args[1]] + 1];
}

void op_get_i(bots_cpu* m) {
    m->registers[m->args[0]] = m->memory[m->args[1]] << 8;
    m->registers[m->args[0]] = m->registers[m->args[0]] | m->memory[m->args[1] + 1];
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
    m->ports[m->args[0]] = m->memory[m->registers[m->args[1]]] << 8;
    m->ports[m->args[0]] = m->registers[m->args[0]] | m->memory[m->registers[m->args[1]] + 1];
}

void op_in_i(bots_cpu* m) {
    m->ports[m->args[0]] = m->memory[m->args[1]] << 8;
    m->ports[m->args[0]] = m->registers[m->args[0]] | m->memory[m->args[1] + 1];
}

void op_int_r(bots_cpu* m) {
    (*int_actions[m->registers[m->args[0]]])(m);
}

void op_int_i(bots_cpu* m) {
    (*int_actions[m->args[0]])(m);
}

/* cmp and all jumps use register 0 for flags */
static void set_compare_flags(bots_cpu* m, uint16_t val_a, uint16_t val_b) {
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
    m->fetch_pc = m->args[0];
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_jls_r(bots_cpu* m) {
    if(m->registers[0] & 0x8000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jls_i(bots_cpu* m) {
    if(m->registers[0] & 0x8000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_r(bots_cpu* m) {
    if(m->registers[0] & 0x4000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jgr_i(bots_cpu* m) {
    if(m->registers[0] & 0x4000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_r(bots_cpu* m) {
    if(!(m->registers[0] & 0x2000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jne_i(bots_cpu* m) {
    if(!(m->registers[0] & 0x2000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_r(bots_cpu* m) {
    if(m->registers[0] & 0x2000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jeq_i(bots_cpu* m) {
    if(m->registers[0] & 0x2000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_r(bots_cpu* m) {
    if(m->registers[0] & 0x6000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jge_i(bots_cpu* m) {
    if(m->registers[0] & 0x6000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_r(bots_cpu* m) {
    if(m->registers[0] & 0xa000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jle_i(bots_cpu* m) {
    if(m->registers[0] & 0xa000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_r(bots_cpu* m) {
    if(m->registers[0] & 0x1000){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jz_i(bots_cpu* m) {
    if(m->registers[0] & 0x1000){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_r(bots_cpu* m) {
    if(!(m->registers[0] & 0x1000)){
        m->fetch_pc = m->registers[m->args[0]];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
}

void op_jnz_i(bots_cpu* m) {
    if(!(m->registers[0] & 0x1000)){
        m->fetch_pc = m->args[0];
        m->decode_ready = 0;
        m->execute_ready = 0;
    }
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
