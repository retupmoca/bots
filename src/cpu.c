#include <bots/cpu.h>
#include <bots/ops.h>

void bots_cpu_execute(bots_cpu* m) {
    if(!m->execute_ready)
        return;
    m->registers[0] = 0;
    m->registers[1] = 1;
    (*bots_cpu_oplist[m->op].execute)(m);
}

void bots_cpu_decode(bots_cpu* m) {
    char i;
    if(!m->decode_ready)
        return;
    
    m->op = m->decode_bytes[0];
    uint8_t pos = 1;
    for(i = 0; i < bots_cpu_oplist[m->op].argcount; i++){
        uint8_t argsize = bots_cpu_oplist[m->op].arg_sizes[i];
        m->args[i] = 0;
        uint8_t j = 0;
        for(; j < argsize; j++){
            m->args[i] <<= 8;
            m->args[i] |= m->decode_bytes[pos++];
        }
    }

    m->execute_ready = 1;
    m->pc = m->decode_pc;
}

void bots_cpu_fetch(bots_cpu* m) {
    if(m->fetch_pc > m->mem_max) {
        m->fetch_pc = 0;
    }
    char op = m->memory[m->fetch_pc];
    char i;
    for(i=0; i < bots_cpu_oplist[op].size; i++){
        if(m->fetch_pc + i <= m->mem_max){
            m->decode_bytes[i] = m->memory[m->fetch_pc + i];
        }
    }
    m->decode_pc = m->fetch_pc;
    m->decode_ready = 1;
    m->fetch_pc += bots_cpu_oplist[op].size;

    if(op == 53 || op == 71) { /* hard jump / call - follow it for prefetching */
        m->fetch_pc = m->decode_bytes[1] << 8;
        m->fetch_pc |= m->decode_bytes[2];
    }
}
