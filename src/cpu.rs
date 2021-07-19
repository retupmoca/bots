pub struct Cpu {
    pub registers: [u16; 12],
    pub memory: [u8; 65536],
    //peripherals: ,
    pub user_mem_max: u16,

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

impl Default for Cpu {
    fn default() -> Cpu {
        Cpu {
            registers: [0; 12],
            memory: [0; 65536],
            user_mem_max: 0,

            fetch_flag: 0,
            fetch_pc: 0,
            fetched_pc: 0,
            fetched_instruction: 0,

            decode_flag: 0,
            decoded_pc: 0,
            decoded_opcode: 0,
            decoded_flags: 0,
            decoded_ra: 0,
            decoded_rb: 0,
            decoded_imm: 0,

            execute_cycle: 0
        }
    }
}
