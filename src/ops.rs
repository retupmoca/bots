use crate::world::Bot;
use crate::cpu::Cpu;

type CpuOp = fn(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool;

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

fn op_nop(_cpu: &mut Cpu, _bot: &Bot, _cycle: u8, _flags: u8, _ra: u8, _rb: u8, _imm: u16) -> bool {
    true
}
fn op_add(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_sub(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_mul(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_div(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_or(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_and(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_xor(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_shift(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_not(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_push(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_pop(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_store(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_load(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_cmp(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_jmp(cpu: &mut Cpu, bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
