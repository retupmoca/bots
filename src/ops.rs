use crate::world::Bot;

type CpuOp = fn(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool;

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

fn op_nop(_bot: &Bot, _cycle: u8, _flags: u8, _ra: u8, _rb: u8, _imm: u16) -> bool {
    true
}
fn op_add(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_sub(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_mul(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_div(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_or(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_and(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_xor(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_shift(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_not(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_push(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_pop(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_store(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_load(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_cmp(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
fn op_jmp(bot: &Bot, cycle: u8, flags: u8, ra: u8, rb: u8, imm: u16) -> bool {
    todo!()
}
