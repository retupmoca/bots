use bitflags::bitflags;
use crate::world::Bot;
use crate::cpu::Cpu;

enum RVInst {
    RType(u32),
    IType(u32),
    SType(u32),
    BType(u32),
    UType(u32),
    JType(u32),
}

type CpuOp = fn(cpu: &mut Cpu, bot: &Bot, cycle: u8, inst: u32) -> bool;

pub const CPU_OPLIST: [(u8, CpuOp); 11] = [
    (0b00_000_11, op_load),
    (0b01_000_11, op_store),
    (0b11_000_11, op_branch),
    (0b11_001_11, op_jalr),
    (0b00_011_11, op_misc_mem),
    (0b11_011_11, op_jal),
    (0b00_100_11, op_op_imm),
    (0b01_100_11, op_op),
    (0b11_100_11, op_system),
    (0b00_101_11, op_auipc),
    (0b01_101_11, op_lui),
];

type AluOp = fn(ina: u32, inb: u32, flag: u8) -> u32;

pub const ALU_OPLIST: [(u8, AluOp); 8] = [
    (0b000, alu_add),
    (0b001, alu_sll),
    (0b010, alu_slt),
    (0b011, alu_sltu),
    (0b100, alu_xor),
    (0b101, alu_srl),
    (0b110, alu_or),
    (0b111, alu_and)
];

fn op_op_imm(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::IType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1()];
    let inb = decoded.imm();

    for op in ALU_OPLIST {
        if op.0 == funct3 {
            if op.0 == 0b101 {
                // srl is a special case for logical vs arithmetic shift
                cpu.registers[decoded.rd()] = op.1(ina, inb & 0x1f, (inb >> 5) > 0);
            }
            else {
                cpu.registers[decoded.rd()] = op.1(ina, inb, 0);
            }
            break;
        }
    }

    true
}

fn op_op(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::RType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1()];
    let inb = cpu.registers[decoded.rs2()];
    let flag = decoded.funct7() > 0;

    for op in ALU_OPLIST {
        if op.0 == funct3 {
            cpu.registers[decoded.rd()] = op.1(ina, inb, flag);
            break;
        }
    }

    true
}

fn alu_add(ina: u32, inb: u32, flag: u8) -> u32 {
    if flag {
        ina - inb
    }
    else {
        ina + inb
    }
}

fn alu_sll(ina: u32, inb: u32, _flag: u8) -> u32 { ina << inb }

fn alu_slt(ina: u32, inb: u32, _flag: u8) -> u32 {
    if ina as i32 < inb as i32 {
        1
    } else {
        0
    }
}

fn alu_sltu(ina: u32, inb: u32, _flag: u8) -> u32 {
    if ina < inb {
        1
    } else {
        0
    }
}

fn alu_xor(ina: u32, inb: u32, _flag: u8) -> u32 { ina ^ inb }

fn alu_srl(ina: u32, inb: u32, flag: u8) -> u32 {
    if flag {
        ina as i32 >> inb
    } else {
        ina >> inb
    }
}

fn alu_or(ina: u32, inb: u32, _flag: u8) -> u32 { ina | inb }

fn alu_and(ina: u32, inb: u32, _flag: u8) -> u32 { ina & inb }

fn op_load(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::IType(inst);
    let target = decoded.imm() + cpu.registers[decoded.rs1()];
    let funct3 = decoded.funct3();
    let size = funct3 & 0b11;
    let raw = if target >= 0xf0_00_00_00 {
        // I/O read
        match size {
            0 => bot.read_peripheral_byte(target) as u32,
            1 => bot.read_peripheral_half(target) as u32,
            2 => bot.read_peripheral_word(target)
        }
    }
    else {
        // memory read
        match size {
            0 => cpu.memory[target] as u32,
            1 => (cpu.memory[target] as u32) << 8 + (cpu.memory[target+1] as u32),
            2 => (cpu.memory[target] as u32) << 24 + (cpu.memory[target+1] as u32) << 16 + (cpu.memory[target+2] as u32) << 8 + (cpu.memory[target+3] as u32),
        }
    }

    cpu.registers[decoded.rd()] = if funct3 & 0b100 {
        match size {
            0 => (raw as i8) as i32,
            1 => (raw as i16) as i32
        }
    }
    else {
        raw
    }
    true
}

fn op_store(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::SType(inst);
    let target = decoded.imm() + cpu.registers[decoded.rs1()];
    let size = decoded.funct3();
    if target >= 0xf0_00_00_00 {
        // I/O write
        match size {
            0 => bot.write_peripheral_byte(target, cpu.registers[decoded.rs2()] as u8),
            1 => bot.write_peripheral_half(target, cpu.registers[decoded.rs2()] as u16),
            2 => bot.write_peripheral_word(target, cpu.registers[decoded.rs2()])
        }
    }
    else {
        // memory write
        let rs2 = decoded.rs2();
        match size {
            0 => cpu.memory[target] = cpu.registers[rs2] as u8,
            1 => {
                cpu.memory[target] = cpu.registers[rs2] >> 8 as u8;
                cpu.memory[target + 1] = cpu.registers[rs2] as u8;
            },
            2 => {
                cpu.memory[target] = cpu.registers[rs2] >> 24 as u8;
                cpu.memory[target + 1] = cpu.registers[rs2] >> 16 as u8;
                cpu.memory[target + 2] = cpu.registers[rs2] >> 8 as u8;
                cpu.memory[target + 3] = cpu.registers[rs2] as u8;
            },
        }
    }
    true
}

type CmpOp = fn(ina: u32, inb: u32) -> bool;

pub const CMP_OPLIST: [(u8, CmpOp); 6] = [
    (0b000, cmp_eq),
    (0b001, cmp_ne),
    (0b100, cmp_lt),
    (0b101, cmp_ge),
    (0b110, cmp_ltu),
    (0b111, cmp_geu)
];

fn op_branch(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::BType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1()];
    let inb = cpu.registers[decoded.rs2()];

    let mut branch = false;

    for op in ALU_OPLIST {
        if op.0 == funct3 {
            branch = op.1(ina, inb, 0);
            break;
        }
    }

    if branch {
        cpu.pc += decoded.imm();
    }

    true
}

fn cmp_eq(ina: u32, inb: u32) -> bool { ina == inb }

fn cmp_ne(ina: u32, inb: u32) -> bool { ina != inb }

fn cmp_lt(ina: u32, inb: u32) -> bool { (ina as i32) < (inb as i32) }

fn cmp_ge(ina: u32, inb: u32) -> bool { (ina as i32) >= (inb as i32) }

fn cmp_ltu(ina: u32, inb: u32) -> bool { ina < inb }

fn cmp_geu(ina: u32, inb: u32) -> bool { ina >= inb }

fn op_jalr(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::IType(inst);

    cpu.registers[decoded.rd()] = cpu.pc + 4;
    cpu.pc = (decoded.imm() + cpu.registers[decoded.rs1()]) & 0xff_ff_ff_fe;

    true
}

fn op_misc_mem(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _inst: u32) -> bool {
    // fences should be a no-op with this simple architecture
    // NOTE: if/when we implement a pipeline simulation, FENCE.I must be implemented
    true
}

fn op_jal(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::JType(inst);

    cpu.registers[decoded.rd()] = cpu.pc + 4;
    cpu.pc = cpu.pc + decoded.imm();

    true
}

fn op_system(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _inst: u32) -> bool {
    // TODO: nop for now. If we stick with rv32i (instead of dropping to rv32e), we should at least
    // implement the required counters.
    //
    // ECALL or EBREAK could potentially end up being useful as well
    true
}

fn op_auipc(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::UType(inst);
    cpu.registers[decoded.rd()] = (decoded.imm() << 12) + cpu.pc;
}

fn op_lui(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RVInst::UType(inst);
    cpu.registers[decoded.rd()] = decoded.imm() << 12
    true
}

//type CpuOp = fn(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool;
//
//const CPU_OP_COUNT: usize = 16;
//pub const CPU_OPLIST: [CpuOp; CPU_OP_COUNT] = [
//    op_nop,
//    op_add,
//    op_sub,
//    op_mul,
//    op_div,
//    op_or,
//    op_and,
//    op_xor,
//    op_shift,
//    op_not,
//    op_push,
//    op_pop,
//    op_store,
//    op_load,
//    op_cmp,
//    op_jmp,
//];
//
//bitflags! {
//    pub struct Flags: u8 {
//        const IMM_IS_REG = 0x08;
//        const IMM_FIRST = 0x04;
//
//        const DIV_REMAINDER = 0x02;
//        const SHIFT_LEFT = 0x02;
//        const STORE_BYTE = 0x04;
//        const LOAD_BYTE = 0x04;
//
//        const JMP_IMM_IS_REG = 0x80;
//        const JMP_IFLESS = 0x10;
//        const JMP_IFEQ = 0x08;
//        const JMP_IFMORE = 0x04;
//        const JMP_IFNOT = 0x02;
//        const JMP_CALL = 0x01;
//    }
//}
//impl Flags {
//    pub fn new(bits: u8) -> Flags {
//        Flags { bits }
//    }
//
//    fn combine_nibbles(a: u8, b: u8) -> Flags {
//        Flags { bits: a << 4 | b & 0x0f }
//    }
//}
//
//type F = Flags;
//
//fn op_nop(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: Flags, _ra: u8, _rb: u8, _imm: u16) -> bool {
//    true
//}
//fn op_add(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] + m.registers[imm as usize]
//        }
//        else {
//            m.registers[rb as usize] + imm
//        };
//    true
//}
//fn op_sub(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] - m.registers[imm as usize]
//        }
//        else if flags.contains(F::IMM_FIRST) {
//            imm - m.registers[rb as usize]
//        }
//        else {
//            m.registers[rb as usize] - imm
//        };
//    true
//}
//fn op_mul(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] * m.registers[imm as usize]
//        }
//        else {
//            m.registers[rb as usize] * imm
//        };
//    true
//}
//fn op_div(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::DIV_REMAINDER) {
//            if flags.contains(F::IMM_IS_REG) {
//                m.registers[rb as usize] % m.registers[imm as usize]
//            }
//            else if flags.contains(F::IMM_FIRST) {
//                imm % m.registers[rb as usize]
//            }
//            else {
//                m.registers[rb as usize] % imm
//            }
//        }
//        else {
//            if flags.contains(F::IMM_IS_REG) {
//                m.registers[rb as usize] / m.registers[imm as usize]
//            }
//            else if flags.contains(F::IMM_FIRST) {
//                imm / m.registers[rb as usize]
//            }
//            else {
//                m.registers[rb as usize] / imm
//            }
//        };
//    true
//}
//fn op_or(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] | m.registers[imm as usize]
//        }
//        else {
//            m.registers[rb as usize] | imm
//        };
//    true
//}
//fn op_and(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] & m.registers[imm as usize]
//        }
//        else {
//            m.registers[rb as usize] & imm
//        };
//    true
//}
//fn op_xor(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::IMM_IS_REG) {
//            m.registers[rb as usize] ^ m.registers[imm as usize]
//        }
//        else {
//            m.registers[rb as usize] ^ imm
//        };
//    true
//}
//fn op_shift(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    m.registers[ra as usize] =
//        if flags.contains(F::SHIFT_LEFT) {
//            if flags.contains(F::IMM_IS_REG) {
//                m.registers[rb as usize] << m.registers[imm as usize]
//            }
//            else if flags.contains(F::IMM_FIRST) {
//                imm << m.registers[rb as usize]
//            }
//            else {
//                m.registers[rb as usize] << imm
//            }
//        }
//        else {
//            if flags.contains(F::IMM_IS_REG) {
//                m.registers[rb as usize] >> m.registers[imm as usize]
//            }
//            else if flags.contains(F::IMM_FIRST) {
//                imm >> m.registers[rb as usize]
//            }
//            else {
//                m.registers[rb as usize] >> imm
//            }
//        };
//    true
//}
//fn op_not(m: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: Flags, ra: u8, rb: u8, _imm: u16) -> bool {
//    m.registers[ra as usize] = !m.registers[rb as usize];
//    true
//}
//// push/pop use register 10 as the stack pointer
//// pointer is assumed to be at the end of usable memory and grows down
//fn op_push(m: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: Flags, ra: u8, _rb: u8, _imm: u16) -> bool {
//    m.registers[10] -= 2;
//    m.memory[m.registers[10] as usize] = (m.registers[ra as usize] >> 8) as u8;
//    m.memory[m.registers[10] as usize + 1] = (m.registers[ra as usize] & 0xff) as u8;
//    true
//}
//fn op_pop(m: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: Flags, ra: u8, _rb: u8, _imm: u16) -> bool {
//    m.registers[ra as usize] = (m.memory[m.registers[10] as usize] as u16) << 8;
//    m.registers[ra as usize] |= m.memory[m.registers[10] as usize + 1] as u16;
//    m.registers[10] += 2;
//    true
//}
//
//fn compute_memory_address(m: &mut Cpu, size: u8, allow_io: bool, imm_is_reg: bool, reg: u8, imm: u16) -> u16 {
//    let mut target: u16 = m.registers[reg as usize];
//    if imm_is_reg {
//        target += m.registers[imm as usize];
//    }
//    else {
//        target += imm;
//    }
//
//    if allow_io {
//        if target > (m.user_mem_max - (size as u16-1)) && target < 0xf000 {
//            target = 0;
//        }
//    }
//    else if target > (m.user_mem_max - (size as u16-1)) {
//        target = 0;
//    }
//
//    target
//}
//
//fn op_store(m: &mut Cpu, bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    if flags.contains(F::STORE_BYTE) {
//        let target = compute_memory_address(m, 1, true, flags.contains(F::IMM_IS_REG), rb, imm);
//        if target >= 0xf000 {
//            bot.write_peripheral_byte(target, (m.registers[ra as usize] & 0xff) as u8);
//        }
//        else {
//            m.memory[target as usize] = (m.registers[ra as usize] & 0xff) as u8;
//        }
//    }
//    else {
//        let target = compute_memory_address(m, 2, true, flags.contains(F::IMM_IS_REG), rb, imm);
//        if target >= 0xf000 {
//            bot.write_peripheral_word(target, m.registers[ra as usize])
//        }
//        else {
//            m.memory[target as usize] = (m.registers[ra as usize] >> 8) as u8;
//            m.memory[target as usize + 1] = (m.registers[ra as usize] & 0xff) as u8;
//        }
//    }
//    true
//}
//fn op_load(m: &mut Cpu, bot: &Bot, _cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    if flags.contains(F::LOAD_BYTE) {
//        let target = compute_memory_address(m, 1, true, flags.contains(F::IMM_IS_REG), rb, imm);
//        if target >= 0xf000 {
//            m.registers[ra as usize] = bot.read_peripheral_byte(target) as u16;
//        }
//        else {
//            m.registers[ra as usize] = m.memory[target as usize] as u16;
//        }
//    }
//    else {
//        let target = compute_memory_address(m, 2, true, flags.contains(F::IMM_IS_REG), rb, imm);
//        if target >= 0xf000 {
//            m.registers[ra as usize] = bot.read_peripheral_word(target);
//        }
//        else {
//            m.registers[ra as usize] = (m.memory[target as usize] as u16) << 8;
//            m.registers[ra as usize] |= m.memory[target as usize + 1] as u16;
//        }
//    }
//    true
//}
//// cmp and all jumps use register 11 for flags
//fn set_compare_flags(m: &mut Cpu, val_a: u16, val_b: u16) {
//    m.registers[11] = m.registers[11] & 0x0fff;
//    
//    if val_a < val_b {
//        m.registers[11] = m.registers[11] | 0x8000;
//    }
//    if val_a > val_b {
//        m.registers[11] = m.registers[11] | 0x4000;
//    }
//    if val_a == val_b {
//        m.registers[11] = m.registers[11] | 0x2000;
//    }
//    if val_a == 0 && val_b == 0 {
//        m.registers[11] = m.registers[11] | 0x1000;
//    }
//}
//fn op_cmp(m: &mut Cpu, _bot: &Bot, _cycle: u8, flags: Flags, _ra: u8, rb: u8, imm: u16) -> bool {
//    if flags.contains(F::IMM_IS_REG) {
//        set_compare_flags(m, m.registers[rb as usize], m.registers[imm as usize]);
//    }
//    else if flags.contains(F::IMM_FIRST) {
//        set_compare_flags(m, imm, m.registers[rb as usize]);
//    }
//    else {
//        set_compare_flags(m, m.registers[rb as usize], imm);
//    }
//    true
//}
//fn op_jmp(m: &mut Cpu, _bot: &Bot, cycle: u8, flags: Flags, ra: u8, rb: u8, imm: u16) -> bool {
//    let flags = F::combine_nibbles(flags.bits, ra & 0x0f);
//    let target = compute_memory_address(m, 4, false, flags.contains(F::JMP_IMM_IS_REG), rb, imm);
//
//    let mut jump = true;
//
//    /* if less and no less-than flag */
//    if flags.contains(F::JMP_IFLESS) && (m.registers[11] & 0x8000) == 0 {
//        jump = false;
//    }
//
//    /* if greater and no greater-than flag */
//    if flags.contains(F::JMP_IFMORE) && (m.registers[11] & 0x4000) == 0 {
//        jump = false;
//    }
//
//    /* if equal and no equal flag */
//    if flags.contains(F::JMP_IFEQ) && (m.registers[11] & 0x2000) == 0 {
//        jump = false;
//    }
//
//    /* check for invert */
//    if flags.contains(F::JMP_IFNOT) {
//        jump = !jump;
//    }
//
//    if jump && cycle == 0 && flags.contains(F::JMP_CALL) { /* if this is the first cycle of a call */
//        m.registers[10] -= 2;
//        let mut pc = m.pc;
//        pc += 4;
//        m.memory[m.registers[10] as usize] = (pc >> 8) as u8;
//        m.memory[m.registers[10] as usize + 1] = (pc & 0xff) as u8;
//        return false;
//    }
//
//    if jump {
//        m.pc = target;
//    }
//
//    true
//}
