use crate::cpu::Peripheral;
use crate::world::{World, Bot};

#[derive(Default)]
pub struct ResetPeripheral;
impl Peripheral for ResetPeripheral {
    fn size(&self) -> u16 { 1 }
    fn read_word(&mut self, _bot: &Bot, _addr: u16) -> u16 { 0 }
    fn read_byte(&mut self, _bot: &Bot, _addr: u16) -> u8 { 0 }
    fn write_word(&mut self, bot: &Bot, addr: u16, val: u16) {
        self.write_byte(bot, addr, (val & 0xff) as u8);
    }
    fn write_byte(&mut self, bot: &Bot, _addr: u16, _val: u8) {
        let mut cpu = bot.cpu.borrow_mut();
        cpu.fetch_pc = 0;
        cpu.fetch_flag = 0;
        cpu.decode_flag = 0;
    }
}

#[derive(Default)]
pub struct RadarPeripheral {
    arc: u8,
    range: u16,
    scan: u8,

    result_range: u16,
    result_offset: u16
}
struct RadarMem;
impl RadarMem {
    const RESULT_OFFSET: u16 = 0x00;
    const RESULT_RANGE: u16 = 0x02;
    const KEEPSHIFT: u16 = 0x04;
    const ARC: u16 = 0x05;
    const RANGE: u16 = 0x06;
    const SCAN: u16 = 0x08;
    const STEERING: u16 = 0x09;
    const TARGET_OFFSET: u16 = 0x11;
}
impl Peripheral for RadarPeripheral {
    fn size(&self) -> u16 { 0x13 }
    fn read_word(&mut self, bot: &Bot, addr: u16) -> u16 { 
        match addr {
            RadarMem::RESULT_OFFSET => self.result_offset,
            RadarMem::RESULT_RANGE => self.result_range,
            RadarMem::RANGE => self.range,
            RadarMem::STEERING => bot.tank_mut()._req_scanner_steering,
            RadarMem::TARGET_OFFSET => ((bot.tank_mut().scanner_offset + bot.tank_mut()._req_scanner_steering as u32) % 1024) as u16,
            _ => 0
        }
    }
    fn read_byte(&mut self, bot: &Bot, addr: u16) -> u8 {
        match addr {
            RadarMem::KEEPSHIFT => bot.tank_mut()._req_scanner_keepshift,
            RadarMem::ARC => self.arc,
            RadarMem::SCAN => self.scan,
            _ => 0
        }
    }
    fn write_word(&mut self, bot: &Bot, addr: u16, val: u16) {
        match addr {
            RadarMem::RANGE => self.range = val,
            RadarMem::STEERING => bot.tank_mut()._req_scanner_steering = val,
            RadarMem::TARGET_OFFSET => bot.tank_mut()._req_scanner_steering = (val as u32 - bot.tank_mut().scanner_offset) as u16,
            _ => {}
        };
    }
    fn write_byte(&mut self, bot: &Bot, addr: u16, val: u8) {
        match addr {
            RadarMem::ARC => self.arc = val,
            RadarMem::SCAN => self.scan = val,
            RadarMem::KEEPSHIFT => bot.tank_mut()._req_scanner_keepshift = val,
            _ => {}
        };
    }
    fn tick(&mut self, _bot: &Bot, _world: &World) {
        // TODO
    }
}

#[derive(Default)]
pub struct TurretPeripheral {
    fire: u8
}
struct TurretMem;
impl TurretMem {
    const FIRE: u16 = 0x00;
    const KEEPSHIFT: u16 = 0x01;
    const STEERING: u16 = 0x02;
    const TARGET_OFFSET: u16 = 0x04;
}
impl Peripheral for TurretPeripheral {
    fn size(&self) -> u16 { 0x06 }
    fn read_word(&mut self, bot: &Bot, addr: u16) -> u16 {
        match addr {
            TurretMem::STEERING => bot.tank_mut()._req_turret_steering,
            TurretMem::TARGET_OFFSET => ((bot.tank_mut().turret_offset + bot.tank_mut()._req_turret_steering as u32) % 1024) as u16,
            _ => 0
        }
    }
    fn read_byte(&mut self, bot: &Bot, addr: u16) -> u8 {
        match addr {
            TurretMem::FIRE => self.fire,
            TurretMem::KEEPSHIFT => bot.tank_mut()._req_turret_keepshift,
            _ => 0
        }
    }
    fn write_word(&mut self, bot: &Bot, addr: u16, val: u16) {
        match addr {
            TurretMem::STEERING => bot.tank_mut()._req_turret_steering = val,
            TurretMem::TARGET_OFFSET => bot.tank_mut()._req_turret_steering = (val as u32 - bot.tank_mut().turret_offset) as u16,
            _ => {}
        };
    }
    fn write_byte(&mut self, bot: &Bot, addr: u16, val: u8) {
        match addr {
            TurretMem::FIRE => self.fire = val,
            TurretMem::KEEPSHIFT => bot.tank_mut()._req_turret_keepshift = val,
            _ => {}
        };
    }
    fn tick(&mut self, _bot: &Bot, _world: &World) {
        // TODO
    }
}

#[derive(Default)]
pub struct HullPeripheral;
struct HullMem;
impl HullMem {
    const THROTTLE: u16 = 0x00;
    const STEERING: u16 = 0x01;
}
impl Peripheral for HullPeripheral {
    fn size(&self) -> u16 { 0x03 }
    fn read_word(&mut self, bot: &Bot, addr: u16) -> u16 {
        match addr {
            HullMem::STEERING => bot.tank_mut()._req_steering,
            _ => 0
        }
    }
    fn read_byte(&mut self, bot: &Bot, addr: u16) -> u8 {
        match addr {
            HullMem::THROTTLE => bot.tank_mut()._req_throttle as u8,
            _ => 0
        }
    }
    fn write_word(&mut self, bot: &Bot, addr: u16, val: u16) {
        match addr {
            HullMem::STEERING => bot.tank_mut()._req_steering = val,
            _ => {}
        };
    }
    fn write_byte(&mut self, bot: &Bot, addr: u16, val: u8) {
        match addr {
            HullMem::THROTTLE => bot.tank_mut()._req_throttle = val as i16,
            _ => {}
        };
    }
}
