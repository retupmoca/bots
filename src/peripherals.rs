use crate::cpu::Peripheral;
use crate::world::{World, Bot};

#[derive(Default)]
pub struct ResetPeripheral;
impl Peripheral for ResetPeripheral {
    fn size(&self) -> u16 { 1 }
    fn read_word(&mut self, _world: &mut World, _bot: &mut Bot, _addr: u16) -> u16 { 0 }
    fn read_byte(&mut self, _world: &mut World, _bot: &mut Bot, _addr: u16) -> u8 { 0 }
    fn write_word(&mut self, world: &mut World, bot: &mut Bot, addr: u16, val: u16) {
        self.write_byte(world, bot, addr, (val & 0xff) as u8);
    }
    fn write_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u8) {
        bot.cpu.fetch_pc = 0;
        bot.cpu.fetch_flag = 0;
        bot.cpu.decode_flag = 0;
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
    const result_offset: u16 = 0x00;
    const result_range: u16 = 0x02;
    const keepshift: u16 = 0x04;
    const arc: u16 = 0x05;
    const range: u16 = 0x06;
    const scan: u16 = 0x08;
    const steering: u16 = 0x09;
    const target_offset: u16 = 0x11;
}
impl Peripheral for RadarPeripheral {
    fn size(&self) -> u16 { 0x13 }
    fn read_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u16 { 
        match addr {
            RadarMem::result_offset => self.result_offset,
            RadarMem::result_range => self.result_range,
            RadarMem::range => self.range,
            RadarMem::steering => bot.tank._req_scanner_steering,
            RadarMem::target_offset => ((bot.tank.scanner_offset + bot.tank._req_scanner_steering as u32) % 1024) as u16,
            _ => 0
        }
    }
    fn read_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u8 {
        match addr {
            RadarMem::keepshift => bot.tank._req_scanner_keepshift,
            RadarMem::arc => self.arc,
            RadarMem::scan => self.scan,
            _ => 0
        }
    }
    fn write_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u16) {
        match addr {
            RadarMem::range => self.range = val,
            RadarMem::steering => bot.tank._req_scanner_steering = val,
            RadarMem::target_offset => bot.tank._req_scanner_steering = (val as u32 - bot.tank.scanner_offset) as u16,
            _ => {}
        };
    }
    fn write_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u8) {
        match addr {
            RadarMem::arc => self.arc = val,
            RadarMem::scan => self.scan = val,
            RadarMem::keepshift => bot.tank._req_scanner_keepshift = val,
            _ => {}
        };
    }
    fn tick(&mut self, _world: &mut World, _bot_idx: &mut Bot) {
        // TODO
    }
}

#[derive(Default)]
pub struct TurretPeripheral {
    fire: u8
}
struct TurretMem;
impl TurretMem {
    const fire: u16 = 0x00;
    const keepshift: u16 = 0x01;
    const steering: u16 = 0x02;
    const target_offset: u16 = 0x04;
}
impl Peripheral for TurretPeripheral {
    fn size(&self) -> u16 { 0x06 }
    fn read_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u16 {
        match addr {
            TurretMem::steering => bot.tank._req_turret_steering,
            TurretMem::target_offset => ((bot.tank.turret_offset + bot.tank._req_turret_steering as u32) % 1024) as u16,
            _ => 0
        }
    }
    fn read_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u8 {
        match addr {
            TurretMem::fire => self.fire,
            TurretMem::keepshift => bot.tank._req_turret_keepshift,
            _ => 0
        }
    }
    fn write_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u16) {
        match addr {
            TurretMem::steering => bot.tank._req_turret_steering = val,
            TurretMem::target_offset => bot.tank._req_turret_steering = (val as u32 - bot.tank.turret_offset) as u16,
            _ => {}
        };
    }
    fn write_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u8) {
        match addr {
            TurretMem::fire => self.fire = val,
            TurretMem::keepshift => bot.tank._req_turret_keepshift = val,
            _ => {}
        };
    }
    fn tick(&mut self, _world: &mut World, _bot_idx: &mut Bot) {
        // TODO
    }
}

#[derive(Default)]
pub struct HullPeripheral;
struct HullMem;
impl HullMem {
    const throttle: u16 = 0x00;
    const steering: u16 = 0x01;
}
impl Peripheral for HullPeripheral {
    fn size(&self) -> u16 { 0x03 }
    fn read_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u16 {
        match addr {
            HullMem::steering => bot.tank._req_steering,
            _ => 0
        }
    }
    fn read_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16) -> u8 {
        match addr {
            HullMem::throttle => bot.tank._req_throttle as u8,
            _ => 0
        }
    }
    fn write_word(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u16) {
        match addr {
            HullMem::steering => bot.tank._req_steering = val,
            _ => {}
        };
    }
    fn write_byte(&mut self, _world: &mut World, bot: &mut Bot, addr: u16, val: u8) {
        match addr {
            HullMem::throttle => bot.tank._req_throttle = val as i16,
            _ => {}
        };
    }
}
