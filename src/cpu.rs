use std::collections::BTreeMap;

use crate::world::{World, Bot};

pub struct Cpu {
    pub registers: [u16; 12],
    pub memory: [u8; 65536],
    pub peripherals: BTreeMap<u16, Box<dyn Peripheral>>,
    pub user_mem_max: u16,

    pub fetch_flag: u8,
    pub fetch_pc: u16,
    fetched_pc: u16,
    fetched_instruction: u32,

    pub decode_flag: u8,
    decoded_pc: u16,
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
            peripherals: BTreeMap::new(),
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

impl Cpu {
    pub fn cycle(&mut self) {
        self.execute();
        self.decode();
        self.fetch();
    }

    fn fetch(&mut self) {
        if self.fetch_pc > self.user_mem_max - 3 {
            self.fetch_pc = 0;
        }

        if self.fetch_flag == 0 {
            self.fetch_flag = 1;
            
            self.fetched_pc = self.fetch_pc;
            self.fetched_instruction  = (self.memory[self.fetch_pc as usize] as u32) << 24;
            self.fetch_pc += 1;
            self.fetched_instruction |= (self.memory[self.fetch_pc as usize] as u32) << 16;
            self.fetch_pc += 1;
            self.fetched_instruction |= (self.memory[self.fetch_pc as usize] as u32) << 8;
            self.fetch_pc += 1;
            self.fetched_instruction |= self.memory[self.fetch_pc as usize] as u32;
            self.fetch_pc += 1;
        }
    }

    fn decode(&mut self) {
        if self.fetch_flag == 0 {
            return;
        }

        if self.decode_flag == 0 {
            self.decode_flag = 1;
            self.fetch_flag = 0;
            self.decoded_pc = self.fetched_pc;
            self.decoded_opcode = ((self.fetched_instruction >> 28) & 0x0f) as u8;
            self.decoded_flags = ((self.fetched_instruction >> 24) & 0x0f) as u8;
            self.decoded_ra = ((self.fetched_instruction >> 20) & 0x0f) as u8;
            self.decoded_rb = ((self.fetched_instruction >> 16) & 0x0f) as u8;
            self.decoded_imm = (self.fetched_instruction & 0xffff) as u16;

            // sanity-check register numbers
            if self.decoded_ra > 11 { self.decoded_ra = 0; }
            if self.decoded_rb > 11 { self.decoded_rb = 0; }
            if (self.decoded_flags & 0x08) != 0 {
                if self.decoded_imm > 11 { self.decoded_imm = 0; }
            }
        }
    }

    fn execute(&mut self) {
        if self.decode_flag == 0 {
            return;
        }

        self.registers[0] = 0;
        self.registers[1] = 1;

        // TODO: actually execute opcode
        let done = false;

        self.execute_cycle += 1;

        if done {
            self.decode_flag = 0;
            self.execute_cycle = 0;
        }
    }

    pub fn mount_peripheral(&mut self, base_addr: u16, peripheral: Box<dyn Peripheral>) {
        self.peripherals.insert(base_addr, peripheral);
    }
}

pub trait Peripheral {
    fn write_word(&mut self, world: &mut World, bot: &mut Bot, addr: u16, val: u16);
    fn write_byte(&mut self, world: &mut World, bot: &mut Bot, addr: u16, val: u8);
    fn read_word(&mut self, world: &mut World, bot: &mut Bot, addr: u16) -> u16;
    fn read_byte(&mut self, world: &mut World, bot: &mut Bot, addr: u16) -> u8;

    fn tick(&mut self, _world: &mut World, _bot: &mut Bot) {}

    fn size(&self) -> u16;
}
