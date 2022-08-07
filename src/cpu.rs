use crate::world::{World, Bot};
use crate::ops::CPU_OPLIST;

pub struct Cpu {
    pub registers: [u32; 32],
    pub pc: u32,
    pub memory: [u8; 65536],
    pub user_mem_max: u32,

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
        self.registers[0] = 0;

        if self.pc > self.user_mem_max - 3 {
            self.pc = 0;
        }

        let mut instruction  = (self.memory[self.pc as usize] as u32);
        instruction |= (self.memory[(self.pc + 1) as usize] as u32) << 8;
        instruction |= (self.memory[(self.pc + 2) as usize] as u32) << 16;
        instruction |= (self.memory[(self.pc + 3) as usize] as u32) << 24;

        let opcode = instruction & 0x7f;

        for op in CPU_OPLIST {
            if op.0 as u32 == opcode {
                op.1(self, bot, 0, instruction);
                break;
            }
        };
    }
}

pub trait Peripheral {
    // TODO: separate word/half/byte implementations might be hard to use from a high level
    // language?
    fn write_word(&mut self, bot: &Bot, addr: u32, val: u32);
    fn write_half(&mut self, bot: &Bot, addr: u32, val: u16);
    fn write_byte(&mut self, bot: &Bot, addr: u32, val: u8);
    fn read_word(&mut self, bot: &Bot, addr: u32) -> u32;
    fn read_half(&mut self, bot: &Bot, addr: u32) -> u16;
    fn read_byte(&mut self, bot: &Bot, addr: u32) -> u8;

    fn tick(&mut self, _bot: &Bot, _world: &World) {}

    fn size(&self) -> u32;
}
