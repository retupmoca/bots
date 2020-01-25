#include <bots/cpu.hpp>
#include <bots/ops.hpp>

static void fetch(bots_cpu *m) {
    if(m->fetch_pc > m->user_mem_max - 3)
        m->fetch_pc = 0;

    if(m->fetch_flag == 0) {
        m->fetch_flag = 1;

        m->fetched_pc = m->fetch_pc;
        m->fetched_instruction  = m->memory[(m->fetch_pc)++] << 24;
        m->fetched_instruction |= m->memory[(m->fetch_pc)++] << 16;
        m->fetched_instruction |= m->memory[(m->fetch_pc)++] << 8;
        m->fetched_instruction |= m->memory[(m->fetch_pc)++];
    }
}

static void decode(bots_cpu *m) {
    if(m->fetch_flag == 0)
        return;

    if(m->decode_flag == 0) {
        m->decode_flag = 1;
        m->fetch_flag = 0;
        m->decoded_pc = m->fetched_pc;
        m->decoded_opcode = (m->fetched_instruction >> 28) & 0x0f;
        m->decoded_flags = (m->fetched_instruction >> 24) & 0x0f;
        m->decoded_ra = (m->fetched_instruction >> 20) & 0x0f;
        m->decoded_rb = (m->fetched_instruction >> 16) & 0x0f;
        m->decoded_imm = m->fetched_instruction & 0xffff;

        /* sanity check register numbers */
        if(m->decoded_ra > 11) m->decoded_ra = 0;
        if(m->decoded_rb > 11) m->decoded_rb = 0;
         /* if the instruction uses immediate as register value, check it */
        if(m->decoded_flags & 0x08)
            if(m->decoded_imm > 11)
                m->decoded_imm = 0;
    }
}

static void execute(bots_cpu *m) {
    if(m->decode_flag == 0)
        return;

    m->registers[0] = 0;
    m->registers[1] = 1;
 
    uint8_t done = (*bots_cpu_oplist[m->decoded_opcode])(
            m,
            m->execute_cycle++,
            m->decoded_flags,
            m->decoded_ra,
            m->decoded_rb,
            m->decoded_imm
    );

    if(done) {
        m->decode_flag = 0;
        m->execute_cycle = 0;
    }
}

void bots_cpu_cycle(bots_cpu *m) {
    execute(m);
    decode(m);
    fetch(m);
}
