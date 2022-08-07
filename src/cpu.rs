use crate::world::{World, Bot};
use crate::ops::CPU_OPLIST;
use crate::ops::Flags;

pub struct Cpu {
    pub registers: [u16; 32],
    pub pc: u16,
    pub memory: [u8; 65536],
    pub user_mem_max: u16,

    execute_cycle: u8,
}

impl Default for Cpu {
    fn default() -> Cpu {
        Cpu {
            registers: [0; 32],
            pc: 0,
            memory: [0; 65536],
            user_mem_max: 0,

            execute_cycle: 0
        }
    }
}

impl Cpu {
    pub fn cycle(&mut self, bot: &Bot) {
        if self.pc > self.user_mem_max - 3 {
            self.pc = 0;
        }

        let mut fetched_instruction  = (self.memory[self.pc as usize] as u32) << 24;
        fetched_instruction |= (self.memory[(self.pc + 1) as usize] as u32) << 16;
        fetched_instruction |= (self.memory[(self.pc + 2) as usize] as u32) << 8;
        fetched_instruction |= self.memory[(self.pc + 3) as usize] as u32;

        let decoded_opcode = ((fetched_instruction >> 28) & 0x0f) as u8;
        let decoded_flags = ((fetched_instruction >> 24) & 0x0f) as u8;
        let mut decoded_ra = ((fetched_instruction >> 20) & 0x0f) as u8;
        let mut decoded_rb = ((fetched_instruction >> 16) & 0x0f) as u8;
        let mut decoded_imm = (fetched_instruction & 0xffff) as u16;

        // sanity-check register numbers
        if decoded_ra > 11 { decoded_ra = 0; }
        if decoded_rb > 11 { decoded_rb = 0; }
        if (decoded_flags & 0x08) != 0 {
            if decoded_imm > 11 { decoded_imm = 0; }
        }

        self.registers[0] = 0;
        self.registers[1] = 1;

        let done = CPU_OPLIST[decoded_opcode as usize](
            self,
            bot,
            self.execute_cycle,
            Flags::new(decoded_flags),
            decoded_ra,
            decoded_rb,
            decoded_imm
        );
        self.execute_cycle += 1;

        if done {
            self.pc += 4;
            self.execute_cycle = 0;
        }
    }
}

pub trait Peripheral {
    fn write_word(&mut self, bot: &Bot, addr: u16, val: u16);
    fn write_byte(&mut self, bot: &Bot, addr: u16, val: u8);
    fn read_word(&mut self, bot: &Bot, addr: u16) -> u16;
    fn read_byte(&mut self, bot: &Bot, addr: u16) -> u8;

    fn tick(&mut self, _bot: &Bot, _world: &World) {}

    fn size(&self) -> u16;
}
