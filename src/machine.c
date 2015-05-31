#include <bots/machine.h>
#include <bots/ops.h>

void machine_execute(machine* m) {
    if(!m->execute_ready)
        return;
    (*oplist[m->op].execute)(m);
}

void machine_decode(machine* m) {
    char i;
    if(!m->decode_ready)
        return;
    
    m->op = m->decode_bytes[0];
    uint8_t pos = 1;
    for(i = 0; i < oplist[m->op].argcount; i++){
        uint8_t argsize = oplist[m->op].arg_sizes[i];
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

void machine_fetch(machine* m) {
    if(m->fetch_pc > m->mem_max) {
        m->fetch_pc = 0;
    }
    char op = m->memory[m->fetch_pc];
    char i;
    for(i=0; i < oplist[op].size; i++){
        if(m->fetch_pc + i <= m->mem_max){
            m->decode_bytes[i] = m->memory[m->fetch_pc + i];
        }
    }
    m->decode_pc = m->fetch_pc;
    m->decode_ready = 1;
    m->fetch_pc += oplist[op].size;
}
