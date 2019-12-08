#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/ops.h>

uint8_t bots_op_nop(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    return 1;
}

uint8_t bots_op_add(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] + m->registers[imm];
    else
        m->registers[ra] = m->registers[rb] + imm;
    return 1;
}

uint8_t bots_op_sub(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register number
     * reverse order of immediate value
     */
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] - m->registers[imm];
    else if (flags & 0x04)
        m->registers[ra] = imm - m->registers[rb];
    else
        m->registers[ra] = m->registers[rb] - imm;
    return 1;
}

uint8_t bots_op_mul(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] * m->registers[imm];
    else
        m->registers[ra] = m->registers[rb] * imm;
    return 1;
}

uint8_t bots_op_div(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register number
     * reverse order of immediate value
     * return remainder instead of normal result
     */
    if(flags & 0x02) {
        if(flags & 0x08)
            m->registers[ra] = m->registers[rb] % m->registers[imm];
        else if (flags & 0x04)
            m->registers[ra] = imm % m->registers[rb];
        else
            m->registers[ra] = m->registers[rb] % imm;
    }
    else {
        if(flags & 0x08)
            m->registers[ra] = m->registers[rb] / m->registers[imm];
        else if (flags & 0x04)
            m->registers[ra] = imm / m->registers[rb];
        else
            m->registers[ra] = m->registers[rb] / imm;
    }
    return 1;
}

uint8_t bots_op_or(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] | m->registers[imm];
    else
        m->registers[ra] = m->registers[rb] | imm;
    return 1;
}

uint8_t bots_op_and(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] & m->registers[imm];
    else
        m->registers[ra] = m->registers[rb] & imm;
    return 1;
}

uint8_t bots_op_xor(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    if(flags & 0x08)
        m->registers[ra] = m->registers[rb] ^ m->registers[imm];
    else
        m->registers[ra] = m->registers[rb] ^ imm;
    return 1;
}

uint8_t bots_op_shift(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register number
     * reverse order of immediate value
     * shift left instead of right
     */
    if(flags & 0x02) {
        if(flags & 0x08)
            m->registers[ra] = m->registers[rb] << m->registers[imm];
        else if (flags & 0x04)
            m->registers[ra] = imm << m->registers[rb];
        else
            m->registers[ra] = m->registers[rb] << imm;
    }
    else {
        if(flags & 0x08)
            m->registers[ra] = m->registers[rb] >> m->registers[imm];
        else if (flags & 0x04)
            m->registers[ra] = imm >> m->registers[rb];
        else
            m->registers[ra] = m->registers[rb] >> imm;
    }
    return 1;
}

uint8_t bots_op_not(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    m->registers[ra] = ~ m->registers[rb];
    return 1;
}

/* push/pop use register 10 as the stack pointer */
/* pointer is assumed to be at the end of usable memory and grows down */
uint8_t bots_op_push(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    m->registers[10] -= 2;
    m->memory[m->registers[10]] = m->registers[ra] >> 8;
    m->memory[m->registers[10] + 1] = m->registers[ra] & 0xff;

    return 1;
}

uint8_t bots_op_pop(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    m->registers[ra]  = m->memory[m->registers[10]] << 8;
    m->registers[ra] |= m->memory[m->registers[10] + 1];
    m->registers[10] += 2;

    return 1;
}

static uint16_t compute_memory_address(
        bots_cpu *m,
        uint8_t size,
        uint8_t allow_io,
        uint8_t flags,
        uint8_t reg,
        uint16_t imm)
{
    uint16_t target = m->registers[reg];
    if(flags & 0x08) /* if using immediate as register */
        target += m->registers[imm];
    else
        target += imm;

    if(allow_io) {
        if(target > (m->user_mem_max - (size-1))
        && target < 0xf000)
            target = 0;
    }
    else if(target > (m->user_mem_max - (size-1)))
        target = 0;

    return target;
}

uint8_t bots_op_store(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register
     * store byte instead of word
     */
    if(flags & 0x04) {
        uint16_t target = compute_memory_address(m, 1, 1, flags, rb, imm);
        m->memory[target] = m->registers[ra] & 0xff;
    }
    else {
        uint16_t target = compute_memory_address(m, 2, 1, flags, rb, imm);
        m->memory[target] = m->registers[ra] >> 8;
        m->memory[target+1] = m->registers[ra] & 0xff;
    }

    return 1;
}

uint8_t bots_op_load(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register
     * load byte instead of word
     */
    if(flags & 0x04) {
        uint16_t target = compute_memory_address(m, 1, 1, flags, rb, imm);
        m->registers[ra] = m->memory[target];
    }
    else {
        uint16_t target = compute_memory_address(m, 2, 1, flags, rb, imm);
        m->registers[ra] = m->memory[target] << 8;
        m->registers[ra] |= m->memory[target+1];
    }

    return 1;
}

/* cmp and all jumps use register 11 for flags */
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

uint8_t bots_op_cmp(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    /* flags:
     * use immediate as register
     * swap compare order if using immediate value
     */
    if(flags & 0x08)
        set_compare_flags(m, m->registers[rb], m->registers[imm]);
    else if(flags & 0x04)
        set_compare_flags(m, imm, m->registers[rb]);
    else
        set_compare_flags(m, m->registers[rb], imm);
    return 1;
}

uint8_t bots_op_jmp(bots_cpu *m, uint8_t cycle, uint8_t flags,
                    uint8_t ra, uint8_t rb, uint16_t imm) {
    uint16_t target = compute_memory_address(m, 4, 0, flags, rb, imm);
    /* flags:
     * use immediate as register
     * jump if zero
     * jump if not zero
     * jump if less
     * jump if equal
     * jump if not equal
     * jump if greater
     * call: push next instruction onto stack
     */

    flags = (flags << 4) | (ra & 0x0f);

    uint8_t jump = 1;

    /* if zero opflag and not 0 cmpflag, don't jump */
    if(flags & 0x40 && !(m->registers[11] & 0x1000))
        jump = 0;
    /* if not zero */
    else if(flags & 0x20 && m->registers[11] & 0x1000)
        jump = 0;
    /* if less */
    else if(flags & 0x10) {
        if (flags & 0x08 && !(m->registers[11] & 0x2000
                              || m->registers[11] & 0x8000))
            jump = 0;
        if(!(flags & 0x08) && !(m->registers[11] & 0x8000))
            jump = 0;
    }
    /* if greater */
    else if(flags & 0x02) {
        if(flags & 0x08 && !(m->registers[11] & 0x4000
                             || m->registers[11] & 0x2000))
            jump = 0;
        if(!(flags & 0x08) && !(m->registers[11] & 0x4000))
            jump = 0;
    }
    /* if equal */
    else if(flags & 0x08 && !(m->registers[11] & 0x2000))
        jump = 0;
    /* if not equal */
    else if(flags & 0x04 && m->registers[11] & 0x2000)
        jump = 0;

    if(jump && !cycle && flags & 0x01) { /* if this is the first cycle of a call */
        m->registers[10] -= 2;
        uint16_t pc = m->decoded_pc;
        pc += 4;
        m->memory[m->registers[10]] = pc >> 8;
        m->memory[m->registers[10] + 1] = pc & 0xff;
        return 0;
    }

    if(!jump)
        target = m->decoded_pc + 4;

    if(m->fetched_pc != target) { /* if prediction was wrong */
        m->fetch_pc = target;
        m->fetch_flag = 0;
        m->decode_flag = 0;
    }

    return 1;
}

bots_cpu_op bots_cpu_oplist[BOTS_CPU_OPCOUNT] = {
    &bots_op_nop,
    &bots_op_add,
    &bots_op_sub,
    &bots_op_mul,
    &bots_op_div,
    &bots_op_or,
    &bots_op_and,
    &bots_op_xor,
    &bots_op_shift,
    &bots_op_not,
    &bots_op_push,
    &bots_op_pop,
    &bots_op_store,
    &bots_op_load,
    &bots_op_cmp,
    &bots_op_jmp
};
