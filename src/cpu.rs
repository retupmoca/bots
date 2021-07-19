pub struct Cpu {
    registers: [u16; 12],
    memory: [u8; 65536],
    //peripherals: ,
    user_mem_max: u16,

    fetch_flag: u8,
    fetch_pc: u16,
    fetched_pc: u16,
    fetched_instruction: u32,

    decode_flag: u8,
    decoded_pc: u8,
    decoded_opcode: u8,
    decoded_flags: u8,
    decoded_ra: u8,
    decoded_rb: u8,
    decoded_imm: u16,

    execute_cycle: u8,
}
