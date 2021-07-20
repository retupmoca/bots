use crate::world::{World, Bot};

type CpuOp = fn(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8;

const CPU_OP_COUNT: usize = 16;
pub const cpu_oplist: [CpuOp; CPU_OP_COUNT] = [
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

fn op_nop(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_add(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_sub(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_mul(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_div(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_or(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_and(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_xor(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_shift(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_not(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_push(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_pop(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_store(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_load(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_cmp(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
fn op_jmp(world: &mut World, bot: &mut Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> u8 {
    todo!()
}
