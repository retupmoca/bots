use std::ptr;
use std::f64::consts::PI;

use crate::cpu::Peripheral;
use crate::world::{World, Bot, Shot, EventType};

#[derive(Default)]
pub struct ResetPeripheral;
impl Peripheral for ResetPeripheral {
    fn size(&self) -> u32 { 1 }
    fn read_mem(&mut self, _bot: &Bot, _addr: u32) -> u32 { 0 }
    fn write_mem(&mut self, bot: &Bot, _addr: u32, val: u32) {
        if val != 0 {
            let mut cpu = bot.cpu.borrow_mut();
            cpu.pc = 0;
        }
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
    const RESULT_OFFSET: u32 = 0x00;
    const RESULT_RANGE: u32 = 0x02;
    const KEEPSHIFT: u32 = 0x04;
    const ARC: u32 = 0x05;
    const RANGE: u32 = 0x06;
    const SCAN: u32 = 0x08;
    const STEERING: u32 = 0x09;
    const TARGET_OFFSET: u32 = 0x11;
}
impl Peripheral for RadarPeripheral {
    fn size(&self) -> u32 { 0x13 }
    fn read_mem(&mut self, bot: &Bot, addr: u32) -> u32 {
        match addr {
            RadarMem::RESULT_OFFSET => self.result_offset as u32,
            RadarMem::RESULT_RANGE => self.result_range as u32,
            RadarMem::RANGE => self.range as u32,
            RadarMem::STEERING => bot.tank_mut()._req_scanner_steering as u32,
            RadarMem::TARGET_OFFSET => ((bot.tank_mut().scanner_offset + bot.tank_mut()._req_scanner_steering as u32) % 1024),
            RadarMem::KEEPSHIFT => bot.tank_mut()._req_scanner_keepshift as u32,
            RadarMem::ARC => self.arc as u32,
            RadarMem::SCAN => self.scan as u32,
            _ => 0
        }
    }
    fn write_mem(&mut self, bot: &Bot, addr: u32, val: u32) {
        match addr {
            RadarMem::RANGE => self.range = val as u16,
            RadarMem::STEERING => bot.tank_mut()._req_scanner_steering = val as u16,
            RadarMem::TARGET_OFFSET => bot.tank_mut()._req_scanner_steering = (val as u32 - bot.tank_mut().scanner_offset) as u16,
            RadarMem::ARC => self.arc = val as u8,
            RadarMem::SCAN => self.scan = val as u8,
            RadarMem::KEEPSHIFT => bot.tank_mut()._req_scanner_keepshift = val as u8,
            _ => {}
        };
    }
    fn tick(&mut self, bot: &Bot, world: &World) {
        let tank = bot.tank_mut();

        if self.scan != 0 {
            self.scan = 0;

            // get global heading of scanner
            let heading = tank.heading + tank.scanner_offset;

            // check angle and range of each bot against scan paramters
            let radar_left = ((heading - self.arc as u32) % 1024) as u16;
            let radar_right = ((heading + self.arc as u32) % 1024) as u16;

            let mut seen_idx = 0;
            let mut seen_bots: [i32; 256] = [0i32; 256];
            let mut seen_bot_range: [i32; 256] = [0i32; 256];
            let mut seen_bot_angle: [i32; 256] = [0i32; 256];

            for (j, wbot) in world.bots.iter().enumerate() {
                if ptr::eq(bot, wbot) {
                    continue;
                }
                let wtank = wbot.tank.borrow();
                if wtank.health <= 0 {
                    continue;
                }

                let x = (wtank.x - tank.x) as f64;
                let y = (wtank.y - tank.y) as f64;

                let angle = ((x.atan2(y) * 512f64 / PI) as i32 % 1024) as i32;
                let range = (y * y + x * x).sqrt() as i32;

                if range <= self.range as i32
                && (  (radar_left < radar_right && angle as u16 > radar_left && (angle as u16) < radar_right)
                    ||(radar_left > radar_right && (angle as u16 > radar_left || (angle as u16) < radar_right))) {
                    seen_bot_range[seen_idx] = range;
                    seen_bot_angle[seen_idx] = angle;
                    seen_bots[seen_idx] = j as i32;
                    seen_idx += 1;
                }
            }

            self.result_range = 0;
            self.result_offset = 0;
            for j in 0..seen_idx {
                if seen_bot_range[j] < self.result_range as i32 || self.result_range == 0 {
                    self.result_range = seen_bot_range[j] as u16;
                    // TODO: some kind of scanner offset instead of bearing
                    self.result_offset = seen_bot_angle[j] as u16;
                }
            }

            world.add_event(EventType::Scan, bot);
        }
    }
}

#[derive(Default)]
pub struct TurretPeripheral {
    fire: u8
}
struct TurretMem;
impl TurretMem {
    const FIRE: u32 = 0x00;
    const KEEPSHIFT: u32 = 0x01;
    const STEERING: u32 = 0x02;
    const TARGET_OFFSET: u32 = 0x04;
}
impl Peripheral for TurretPeripheral {
    fn size(&self) -> u32 { 0x06 }
    fn read_mem(&mut self, bot: &Bot, addr: u32) -> u32 {
        match addr {
            TurretMem::STEERING => bot.tank_mut()._req_turret_steering as u32,
            TurretMem::TARGET_OFFSET => ((bot.tank_mut().turret_offset + bot.tank_mut()._req_turret_steering as u32) % 1024),
            TurretMem::FIRE => self.fire as u32,
            TurretMem::KEEPSHIFT => bot.tank_mut()._req_turret_keepshift as u32,
            _ => 0
        }
    }
    fn write_mem(&mut self, bot: &Bot, addr: u32, val: u32) {
        match addr {
            TurretMem::STEERING => bot.tank_mut()._req_turret_steering = val as u16,
            TurretMem::TARGET_OFFSET => bot.tank_mut()._req_turret_steering = (val as u32 - bot.tank_mut().turret_offset) as u16,
            TurretMem::FIRE => self.fire = val as u8,
            TurretMem::KEEPSHIFT => bot.tank_mut()._req_turret_keepshift = val as u8,
            _ => {}
        };
    }
    fn tick(&mut self, bot: &Bot, world: &World) {
        let tank = bot.tank_mut();

        if self.fire != 0 {
            self.fire = 0;

            let heading = tank.heading + tank.turret_offset;
            let mut x = tank.x;
            let mut y = tank.y;

            let rangle = heading as f64 * PI / 512f64;
            let dist = 65f64;
            let dy = (0.5 + (dist * rangle.cos())).floor();
            let dx = (0.5 + (dist * rangle.sin())).floor();
            x += dx as i32;
            y += dy as i32;

            world.add_shot(Shot {
                x, y, heading
            });
            world.add_event(EventType::Fire, bot);
        }
    }
}

#[derive(Default)]
pub struct HullPeripheral;
struct HullMem;
impl HullMem {
    const THROTTLE: u32 = 0x00;
    const STEERING: u32 = 0x01;
}
impl Peripheral for HullPeripheral {
    fn size(&self) -> u32 { 0x03 }
    fn read_mem(&mut self, bot: &Bot, addr: u32) -> u32 {
        match addr {
            HullMem::STEERING => bot.tank_mut()._req_steering as u32,
            HullMem::THROTTLE => bot.tank_mut()._req_throttle as u32,
            _ => 0
        }
    }
    fn write_mem(&mut self, bot: &Bot, addr: u32, val: u32) {
        match addr {
            HullMem::STEERING => bot.tank_mut()._req_steering = val as u16,
            HullMem::THROTTLE => bot.tank_mut()._req_throttle = val as i16,
            _ => {}
        };
    }
}
