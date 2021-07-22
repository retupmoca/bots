use crate::world::Bot;
use crate::cpu::Cpu;

type CpuOp = fn(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool;

const CPU_OP_COUNT: usize = 16;
pub const CPU_OPLIST: [CpuOp; CPU_OP_COUNT] = [
    op_nop,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_or,
    op_and,
    op_xor,
    op_shift,
    op_not,
    op_push,
    op_pop,
    op_store,
    op_load,
    op_cmp,
    op_jmp,
];

bitflags! {
    pub struct Flags: u8 {
        const IMM_IS_REG = 0x08;
        const IMM_FIRST = 0x04;

        const DIV_REMAINDER = 0x02;
        const SHIFT_LEFT = 0x02;
        const STORE_BYTE = 0x04;
        const LOAD_BYTE = 0x04;

        const JMP_IMM_IS_REG = 0x80;
        const JMP_IFLESS = 0x10;
        const JMP_IFEQ = 0x08;
        const JMP_IFMORE = 0x04;
        const JMP_IFNOT = 0x02;
        const JMP_CALL = 0x01;
    }
}
impl Flags {
    pub fn new(bits: u8) -> Flags {
        Flags { bits }
    }

    fn combine_nibbles(a: u8, b: u8) -> Flags {
        Flags { bits: a << 4 | b & 0x0f }
    }
}

type F = Flags;

fn op_nop(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: Flags, _ra: u8, _rb: u8, _imm: u16) -> bool {
    true
}
fn op_add(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] + m.registers[imm as usize]
        }
        else {
            m.registers[rb as usize] + imm
        };
    true
}
fn op_sub(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] - m.registers[imm as usize]
        }
        else if flags.contains(F::IMM_FIRST) {
            imm - m.registers[rb as usize]
        }
        else {
            m.registers[rb as usize] - imm
        };
    true
}
fn op_mul(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] * m.registers[imm as usize]
        }
        else {
            m.registers[rb as usize] * imm
        };
    true
}
fn op_div(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::DIV_REMAINDER) {
            if flags.contains(F::IMM_IS_REG) {
                m.registers[rb as usize] % m.registers[imm as usize]
            }
            else if flags.contains(F::IMM_FIRST) {
                imm % m.registers[rb as usize]
            }
            else {
                m.registers[rb as usize] % imm
            }
        }
        else {
            if flags.contains(F::IMM_IS_REG) {
                m.registers[rb as usize] / m.registers[imm as usize]
            }
            else if flags.contains(F::IMM_FIRST) {
                imm / m.registers[rb as usize]
            }
            else {
                m.registers[rb as usize] / imm
            }
        };
    true
}
fn op_or(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] | m.registers[imm as usize]
        }
        else {
            m.registers[rb as usize] | imm
        };
    true
}
fn op_and(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] & m.registers[imm as usize]
        }
        else {
            m.registers[rb as usize] & imm
        };
    true
}
fn op_xor(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::IMM_IS_REG) {
            m.registers[rb as usize] ^ m.registers[imm as usize]
        }
        else {
            m.registers[rb as usize] ^ imm
        };
    true
}
fn op_shift(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] =
        if flags.contains(F::SHIFT_LEFT) {
            if flags.contains(F::IMM_IS_REG) {
                m.registers[rb as usize] << m.registers[imm as usize]
            }
            else if flags.contains(F::IMM_FIRST) {
                imm << m.registers[rb as usize]
            }
            else {
                m.registers[rb as usize] << imm
            }
        }
        else {
            if flags.contains(F::IMM_IS_REG) {
                m.registers[rb as usize] >> m.registers[imm as usize]
            }
            else if flags.contains(F::IMM_FIRST) {
                imm >> m.registers[rb as usize]
            }
            else {
                m.registers[rb as usize] >> imm
            }
        };
    true
}
fn op_not(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] = !m.registers[rb as usize];
    true
}
// push/pop use register 10 as the stack pointer
// pointer is assumed to be at the end of usable memory and grows down
fn op_push(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[10] -= 2;
    m.memory[m.registers[10] as usize] = (m.registers[ra as usize] >> 8) as u8;
    m.memory[m.registers[10] as usize + 1] = (m.registers[ra as usize] & 0xff) as u8;
    true
}
fn op_pop(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    m.registers[ra as usize] = (m.memory[m.registers[10] as usize] as u16) << 8;
    m.registers[ra as usize] |= m.memory[m.registers[10] as usize + 1] as u16;
    m.registers[10] += 2;
    true
}

fn compute_memory_address(m: &mut Cpu, size: u8, allow_io: bool, imm_is_reg: bool, reg: u8, imm: u16) -> u16 {
    let mut target: u16 = m.registers[reg as usize];
    if(imm_is_reg) {
        target += m.registers[imm as usize];
    }
    else {
        target += imm;
    }

    if(allow_io) {
        if(target > (m.user_mem_max - (size as u16-1))
        && target < 0xf000) {
            target = 0;
        }
    }
    else if(target > (m.user_mem_max - (size as u16-1))) {
        target = 0;
    }

    target
}

fn op_store(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    if flags.contains(F::STORE_BYTE) {
        let target = compute_memory_address(m, 1, true, flags.contains(F::IMM_IS_REG), rb, imm);
        if target >= 0xf000 {
            bot.write_peripheral_byte(target, (m.registers[ra as usize] & 0xff) as u8);
        }
        else {
            m.memory[target as usize] = (m.registers[ra as usize] & 0xff) as u8;
        }
    }
    else {
        let target = compute_memory_address(m, 2, true, flags.contains(F::IMM_IS_REG), rb, imm);
        if target >= 0xf000 {
            bot.write_peripheral_word(target, m.registers[ra as usize])
        }
        else {
            m.memory[target as usize] = (m.registers[ra as usize] >> 8) as u8;
            m.memory[target as usize + 1] = (m.registers[ra as usize] & 0xff) as u8;
        }
    }
    true
}
fn op_load(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    if flags.contains(F::LOAD_BYTE) {
        let target = compute_memory_address(m, 1, true, flags.contains(F::IMM_IS_REG), rb, imm);
        if target >= 0xf000 {
            m.registers[ra as usize] = bot.read_peripheral_byte(target) as u16;
        }
        else {
            m.registers[ra as usize] = m.memory[target as usize] as u16;
        }
    }
    else {
        let target = compute_memory_address(m, 2, true, flags.contains(F::IMM_IS_REG), rb, imm);
        if target >= 0xf000 {
            m.registers[ra as usize] = bot.read_peripheral_word(target);
        }
        else {
            m.registers[ra as usize] = (m.memory[target as usize] as u16) << 8;
            m.registers[ra as usize] |= m.memory[target as usize + 1] as u16;
        }
    }
    true
}
// cmp and all jumps use register 11 for flags
fn set_compare_flags(m: &mut Cpu, val_a: u16, val_b: u16) {
    m.registers[11] = m.registers[11] & 0x0fff;
    
    if(val_a < val_b) {
        m.registers[11] = m.registers[11] | 0x8000;
    }
    if(val_a > val_b) {
        m.registers[11] = m.registers[11] | 0x4000;
    }
    if(val_a == val_b) {
        m.registers[11] = m.registers[11] | 0x2000;
    }
    if(val_a == 0 && val_b == 0) {
        m.registers[11] = m.registers[11] | 0x1000;
    }
}
fn op_cmp(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    if flags.contains(F::IMM_IS_REG) {
        set_compare_flags(m, m.registers[rb as usize], m.registers[imm as usize]);
    }
    else if flags.contains(F::IMM_FIRST) {
        set_compare_flags(m, imm, m.registers[rb as usize]);
    }
    else {
        set_compare_flags(m, m.registers[rb as usize], imm);
    }
    true
}
fn op_jmp(m: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
    let flags = F::combine_nibbles(flags.bits, ra & 0x0f);
    let mut target = compute_memory_address(m, 4, false, flags.contains(F::JMP_IMM_IS_REG), rb, imm);

    let mut jump = true;

    /* if less and no less-than flag */
    if(flags.contains(F::JMP_IFLESS) && (m.registers[11] & 0x8000) == 0) {
        jump = false;
    }

    /* if greater and no greater-than flag */
    if(flags.contains(F::JMP_IFMORE) && (m.registers[11] & 0x4000) == 0) {
        jump = false;
    }

    /* if equal and no equal flag */
    if(flags.contains(F::JMP_IFEQ) && (m.registers[11] & 0x2000) == 0) {
        jump = false;
    }

    /* check for invert */
    if(flags.contains(F::JMP_IFNOT)) {
        jump = !jump;
    }

    if(jump && cycle == 0 && flags.contains(F::JMP_CALL)) { /* if this is the first cycle of a call */
        m.registers[10] -= 2;
        let mut pc = m.decoded_pc;
        pc += 4;
        m.memory[m.registers[10] as usize] = (pc >> 8) as u8;
        m.memory[m.registers[10] as usize + 1] = (pc & 0xff) as u8;
        return false;
    }

    if(!jump) {
        target = m.decoded_pc + 4;
    }

    if(m.fetched_pc != target) { /* if prediction was wrong */
        m.fetch_pc = target;
        m.fetch_flag = 0;
        m.decode_flag = 0;
    }

    true
}
