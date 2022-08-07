use crate::world::Bot;
use crate::cpu::Cpu;

struct RType(u32);
impl RType {
    fn funct7(&self) -> u8 {
        ((self.0 & 0xfe_00_00_00) >> 21) as u8
    }

    fn rs2(&self) -> u8 {
        ((self.0 & 0x01_f0_00_00) >> 20) as u8
    }

    fn rs1(&self) -> u8 {
        ((self.0 & 0x00_0f_80_00) >> 15) as u8
    }

    fn funct3(&self) -> u8 {
        ((self.0 & 0x00_00_70_00) >> 12) as u8
    }

    fn rd(&self) -> u8 {
        ((self.0 & 0x00_00_0f_ff) >> 7) as u8
    }
}
struct IType(u32);
impl IType {
    fn imm(&self) -> u32 {
        (((self.0 & 0xff_f0_00_00) as i32) >> 20) as u32
    }

    fn rs1(&self) -> u8 {
        ((self.0 & 0x00_0f_80_00) >> 15) as u8
    }

    fn funct3(&self) -> u8 {
        ((self.0 & 0x00_00_70_00) >> 12) as u8
    }

    fn rd(&self) -> u8 {
        ((self.0 & 0x00_00_0f_ff) >> 7) as u8
    }
}
struct SType(u32);
impl SType {
    fn imm(&self) -> u32 {
        (((((self.0 & 0xfe_00_00_00) as i32) >> 25) as u32) << 5)
        + ((self.0 & 0x00_00_0f_80) >> 7)
    }

    fn rs2(&self) -> u8 {
        ((self.0 & 0x01_f0_00_00) >> 20) as u8
    }

    fn rs1(&self) -> u8 {
        ((self.0 & 0x00_0f_80_00) >> 15) as u8
    }

    fn funct3(&self) -> u8 {
        ((self.0 & 0x00_00_70_00) >> 12) as u8
    }
}
struct BType(u32);
impl BType {
    fn imm(&self) -> u32 {
        (((((self.0 & 0x80_00_00_00) as i32) >> 31) as u32) << 12)
        + (((self.0 & 0x00_00_00_80) >> 7) << 11)
        + (((self.0 & 0x7e_00_00_00) >> 25) << 5)
        + (((self.0 & 0x00_00_0f_00) >> 8) << 1)
    }

    fn rs2(&self) -> u8 {
        ((self.0 & 0x01_f0_00_00) >> 20) as u8
    }

    fn rs1(&self) -> u8 {
        ((self.0 & 0x00_0f_80_00) >> 15) as u8
    }

    fn funct3(&self) -> u8 {
        ((self.0 & 0x00_00_70_00) >> 12) as u8
    }
}
struct UType(u32);
impl UType {
    fn imm(&self) -> u32 {
        self.0 & 0xff_ff_f0_00
    }

    fn rd(&self) -> u8 {
        ((self.0 & 0x00_00_0f_ff) >> 7) as u8
    }
}
struct JType(u32);
impl JType {
    fn imm(&self) -> u32 {
        ((((self.0 & 0x80_00_00_00) as i32) >> 31) as u32).wrapping_shl(20)
        + ((self.0 & 0x00_0f_f0_00) >> 12).wrapping_shl(12)
        + ((self.0 & 0x00_10_00_00) >> 20).wrapping_shl(11)
        + ((self.0 & 0x7f_e0_00_00) >> 21).wrapping_shl(1)
    }

    fn rd(&self) -> u8 {
        ((self.0 & 0x00_00_0f_ff) >> 7) as u8
    }
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

type AluOp = fn(ina: u32, inb: u32, flag: bool) -> u32;

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
    let decoded = IType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1() as usize];
    let inb = decoded.imm();

    for op in ALU_OPLIST {
        if op.0 == funct3 {
            if op.0 == 0b101 {
                // srl is a special case for logical vs arithmetic shift
                cpu.registers[decoded.rd() as usize] = op.1(ina, inb & 0x1f, (inb >> 5) > 0);
            }
            else {
                cpu.registers[decoded.rd() as usize] = op.1(ina, inb, false);
            }
            break;
        }
    }

    cpu.pc += 4;

    true
}

fn op_op(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = RType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1() as usize];
    let inb = cpu.registers[decoded.rs2() as usize];
    let flag = decoded.funct7() > 0;

    for op in ALU_OPLIST {
        if op.0 == funct3 {
            cpu.registers[decoded.rd() as usize] = op.1(ina, inb, flag);
            break;
        }
    }

    cpu.pc += 4;

    true
}

fn alu_add(ina: u32, inb: u32, flag: bool) -> u32 {
    if flag {
        ina - inb
    }
    else {
        ina + inb
    }
}

fn alu_sll(ina: u32, inb: u32, _flag: bool) -> u32 { ina << inb }

fn alu_slt(ina: u32, inb: u32, _flag: bool) -> u32 {
    if (ina as i32) < (inb as i32) {
        1
    } else {
        0
    }
}

fn alu_sltu(ina: u32, inb: u32, _flag: bool) -> u32 {
    if ina < inb {
        1
    } else {
        0
    }
}

fn alu_xor(ina: u32, inb: u32, _flag: bool) -> u32 { ina ^ inb }

fn alu_srl(ina: u32, inb: u32, flag: bool) -> u32 {
    if flag {
        (ina as i32 >> inb) as u32
    } else {
        ina >> inb
    }
}

fn alu_or(ina: u32, inb: u32, _flag: bool) -> u32 { ina | inb }

fn alu_and(ina: u32, inb: u32, _flag: bool) -> u32 { ina & inb }

fn op_load(cpu: &mut Cpu, bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = IType(inst);
    let target = (decoded.imm() + cpu.registers[decoded.rs1() as usize]) as usize;
    let funct3 = decoded.funct3();
    let size = funct3 & 0b11;
    let raw = if target >= 0xf0_00_00_00 {
        // I/O read
        match size {
            0 => bot.read_peripheral_byte(target as u32) as u32,
            1 => bot.read_peripheral_half(target as u32) as u32,
            2 => bot.read_peripheral_word(target as u32),
            _ => unreachable!()
        }
    }
    else {
        // memory read
        match size {
            0 => cpu.memory[target] as u32,
            1 => (cpu.memory[target] as u32) + ((cpu.memory[target+1] as u32) << 8),
            2 => (cpu.memory[target] as u32) + ((cpu.memory[target+1] as u32) << 8) + ((cpu.memory[target+2] as u32) << 16) + ((cpu.memory[target+3] as u32) << 24),
            _ => unreachable!()
        }
    };

    cpu.registers[decoded.rd() as usize] = if (funct3 & 0b100) > 0 {
        match size {
            0 => (raw as i8) as i32 as u32,
            1 => (raw as i16) as i32 as u32,
            _ => unreachable!()
        }
    }
    else {
        raw
    };

    cpu.pc += 4;
    true
}

fn op_store(cpu: &mut Cpu, bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = SType(inst);
    let target = (decoded.imm() + cpu.registers[decoded.rs1() as usize]) as usize;
    let size = decoded.funct3();
    let rs2 = decoded.rs2() as usize;
    if target >= 0xf0_00_00_00 {
        // I/O write
        match size {
            0 => bot.write_peripheral_byte(target as u32, cpu.registers[rs2] as u8),
            1 => bot.write_peripheral_half(target as u32, cpu.registers[rs2] as u16),
            2 => bot.write_peripheral_word(target as u32, cpu.registers[rs2]),
            _ => unreachable!()
        }
    }
    else {
        // memory write
        match size {
            0 => cpu.memory[target] = cpu.registers[rs2] as u8,
            1 => {
                cpu.memory[target + 0] = (cpu.registers[rs2]) as u8;
                cpu.memory[target + 1] = (cpu.registers[rs2] >> 8) as u8;
            },
            2 => {
                cpu.memory[target + 0] = (cpu.registers[rs2]) as u8;
                cpu.memory[target + 1] = (cpu.registers[rs2] >> 8) as u8;
                cpu.memory[target + 2] = (cpu.registers[rs2] >> 16) as u8;
                cpu.memory[target + 3] = (cpu.registers[rs2] >> 24) as u8;
            },
            _ => unreachable!()
        }
    }

    cpu.pc += 4;
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

fn op_branch(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = BType(inst);
    let funct3 = decoded.funct3();
    let ina = cpu.registers[decoded.rs1() as usize];
    let inb = cpu.registers[decoded.rs2() as usize];

    let mut branch = false;

    for op in CMP_OPLIST {
        if op.0 == funct3 {
            branch = op.1(ina, inb);
            break;
        }
    }

    if branch {
        cpu.pc += decoded.imm();
    }
    else {
        cpu.pc += 4;
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
    let decoded = IType(inst);

    cpu.registers[decoded.rd() as usize] = cpu.pc + 4;
    cpu.pc = (decoded.imm() + cpu.registers[decoded.rs1() as usize]) & 0xff_ff_ff_fe;

    true
}

fn op_misc_mem(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _inst: u32) -> bool {
    // fences should be a no-op with this simple architecture
    // NOTE: if/when we implement a pipeline simulation, FENCE.I must be implemented

    cpu.pc += 4;
    true
}

fn op_jal(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = JType(inst);

    cpu.registers[decoded.rd() as usize] = cpu.pc + 4;
    cpu.pc = cpu.pc.wrapping_add(decoded.imm());

    true
}

fn op_system(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _inst: u32) -> bool {
    // TODO: nop for now. If we stick with rv32i (instead of dropping to rv32e), we should at least
    // implement the required counters.
    //
    // ECALL or EBREAK could potentially end up being useful as well

    cpu.pc += 4;
    true
}

fn op_auipc(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = UType(inst);
    cpu.registers[decoded.rd() as usize] = decoded.imm() + cpu.pc;

    cpu.pc += 4;
    true
}

fn op_lui(cpu: &mut Cpu, _bot: &Bot, _cycle: u8, inst: u32) -> bool {
    let decoded = UType(inst);
    cpu.registers[decoded.rd() as usize] = decoded.imm();

    cpu.pc += 4;
    true
}
