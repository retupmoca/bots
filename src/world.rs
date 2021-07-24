use std::collections::BTreeMap;
#[cfg(not(feature="wasm_ffi"))]
use std::path::Path;
#[cfg(not(feature="wasm_ffi"))]
use std::fs::File;
use std::io::Read;
use std::f64::consts::PI;
use std::cell::RefCell;
use std::ptr;

#[cfg(feature = "wasm_ffi")]
use wasm_bindgen::prelude::*;

use crate::cpu::{Cpu, Peripheral};
use crate::peripherals::*;

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[cfg_attr(any(feature="c_ffi", feature="wasm_ffi"), repr(C))]
#[derive(Clone)]
pub struct WorldConfig {
    pub cpus_per_tick: u8,
    pub spawn_distance: i32,

    pub hull_turn_rate: u16,
    pub turret_turn_rate: u16,
    pub scanner_turn_rate: u16
}

impl Default for WorldConfig {
    fn default() -> Self {
        WorldConfig {
            cpus_per_tick: 2,
            spawn_distance: 100,

            hull_turn_rate: 1,
            turret_turn_rate: 2,
            scanner_turn_rate: 8,
        }
    }
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[cfg_attr(any(feature="c_ffi", feature="wasm_ffi"), repr(C))]
#[derive(Debug)]
pub enum EventType {
    Fire,
    Death,
    Scan,
    Hit,
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[cfg_attr(any(feature="c_ffi", feature="wasm_ffi"), repr(C))]
#[derive(Debug)]
pub struct Event {
    event_type: EventType,
    bot_idx: usize
}

pub struct World {
    config: WorldConfig,
    pub bots: Vec<Bot>,
    shots: RefCell<Vec<Shot>>,
    pub events: RefCell<Vec<Event>>,
}

impl World {
    pub fn new(config: WorldConfig) -> Self {
        World {
            config,
            bots: vec![],
            shots: RefCell::new(vec![]),
            events: RefCell::new(vec![]),
        }
    }

    #[cfg(not(feature="wasm_ffi"))]
    pub fn add_bot(&mut self, filename: &Path) {
        self.bots.push(Bot::from(filename));
    }

    pub fn add_bot_from_data(&mut self, data: &[u8]) {
        self.bots.push(Bot::from(data));
    }

    pub fn add_event(&self, event_type: EventType, bot: &Bot) {
        self.events.borrow_mut().push(Event {
            event_type,
            bot_idx: self.bots.iter().enumerate().filter(|(_, b)| ptr::eq(bot, *b)).next().unwrap().0
        });
    }

    pub fn place_bots(&mut self) {
        let dist = self.config.spawn_distance;

        let mut angle = 0f64;
        let step = (2f64 * PI) / self.bots.len() as f64;

        for bot in &mut self.bots {
            let loc_x = (dist as f64 * angle.cos() + 0.5).floor() as i32;
            let loc_y = (dist as f64 * angle.sin() + 0.5).floor() as i32;
            bot.tank_mut().x = loc_x;
            bot.tank_mut().y = loc_y;
            angle += step;
        }
    }

    pub fn tick(&mut self) {
        self.events.borrow_mut().clear();
        
        self.physics_tick();
        self.process_tick();
    }

    fn physics_tick(&mut self) {
        /* run world physics */
        /* shots */
        let mut shots_idx_to_delete: Vec<usize> = Vec::new();
        for (i, shot) in self.shots.borrow_mut().iter_mut().enumerate() {
            let mut hit = false;
            for bot in self.bots.iter() {
                let mut tank = bot.tank_mut();
                if shot.x >= tank.x - 40
                && shot.x <= tank.x + 40
                && shot.y >= tank.y - 40
                && shot.y <= tank.y + 40
                && tank.health > 0 {
                    hit = true;
                    self.add_event(EventType::Hit, bot);
                    tank.health -= 10;
                    if tank.health <= 0 {
                        tank.health = 0;
                        self.add_event(EventType::Death, bot);
                    }

                    shots_idx_to_delete.push(i);
                    break;
                }
            }
            if hit { continue; }

            let rangle = shot.heading as f64 * PI / 512f64;
            let dist = 20f64;
            let dy = (dist * rangle.cos() + 0.5).floor() as i32;
            let dx = (dist * rangle.sin() + 0.5).floor() as i32;
            let start_x = shot.x;
            let start_y = shot.y;
            shot.x += dx;
            shot.y += dy;
            if (start_x - shot.x).abs() > dx * 2 || (start_y - shot.y) > dy * 2 {
                // detect wrapping and delete shot
                // TODO: rust probably just blows up here?
                shots_idx_to_delete.push(i);
                continue;
            }

            /* check collision again */
            for bot in self.bots.iter() {
                let mut tank = bot.tank_mut();
                if shot.x >= tank.x - 40
                && shot.x <= tank.x + 40
                && shot.y >= tank.y - 40
                && shot.y <= tank.y + 40
                && tank.health > 0 {
                    self.add_event(EventType::Hit, bot);
                    tank.health -= 10;
                    if tank.health <= 0 {
                        tank.health = 0;
                        self.add_event(EventType::Death, bot);
                    }

                    shots_idx_to_delete.push(i);
                    break;
                }
            }
        }
        {
            let mut shots = self.shots.borrow_mut();
            shots_idx_to_delete.reverse();
            for idx in shots_idx_to_delete {
                shots.remove(idx);
            }
        }
        /* bots */
        for bot in &mut self.bots {
            let mut tank = bot.tank_mut();
            if tank.health <= 0 {
                continue;
            }

            /* turn, etc */
            let steering = tank._req_steering;
            let mut real_steering = steering;
            if real_steering <= 512 && real_steering > self.config.hull_turn_rate {
                real_steering = self.config.hull_turn_rate;
            }
            if real_steering > 512 && real_steering < (1024 - self.config.hull_turn_rate) {
                real_steering = 1024 - self.config.hull_turn_rate;
            }
            tank._req_steering -= real_steering;

            tank.heading = (tank.heading + real_steering as u32) % 1024;
            tank.speed = tank._req_throttle as i32;
            if tank.speed > 100 {
                tank.speed = 100;
            }

            /* drive! */
            let rangle = tank.heading as f64 * PI / 512f64;
            let dist = tank.speed / 100 * 6;
            let dy = (dist as f64 * rangle.cos() + 0.5).floor() as i32;
            let dx = (dist as f64 * rangle.sin() + 0.5).floor() as i32;
            tank.x += dx;
            tank.y += dy;

            /* turn turret */
            let mut turret_steering = tank._req_turret_steering;
            if tank._req_turret_keepshift != 0 {
                turret_steering = (turret_steering + 1024 - real_steering) % 1024;
            }
            turret_steering = turret_steering % 1024;

            let mut real_turret_steering = turret_steering;
            if real_turret_steering <= 512 && real_turret_steering > self.config.turret_turn_rate {
                real_turret_steering = self.config.turret_turn_rate;
            }
            if real_turret_steering > 512 && real_turret_steering < (1024 - self.config.turret_turn_rate) {
                real_turret_steering = 1024 - self.config.turret_turn_rate;
            }
            tank._req_turret_steering -= real_turret_steering;

            tank.turret_offset = (tank.turret_offset + real_turret_steering as u32) % 1024;

            /* turn scanner */
            let mut scanner_steering = tank._req_scanner_steering;
            if tank._req_scanner_keepshift != 0 {
                scanner_steering = (scanner_steering + 1024 - real_steering) % 1024;
            }
            scanner_steering = scanner_steering % 1024;

            let mut real_scanner_steering = scanner_steering;
            if real_scanner_steering <= 512 && real_scanner_steering > self.config.scanner_turn_rate {
                real_scanner_steering = self.config.scanner_turn_rate;
            }
            if real_scanner_steering > 512 && real_scanner_steering < (1024 - self.config.scanner_turn_rate) {
                real_scanner_steering = 1024 - self.config.scanner_turn_rate;
            }
            tank._req_scanner_steering -= real_scanner_steering;

            tank.scanner_offset = (tank.scanner_offset + real_scanner_steering as u32) % 1024;
        }
    }

    fn process_tick(&mut self) {
        for bot in self.bots.iter() {
            {
                let tank = bot.tank_mut();
                if tank.health <= 0 {
                    continue;
                }
            }

            for _ in 0..self.config.cpus_per_tick {
                bot.tick_cpu();
            }

            bot.tick_peripherals(self);
        }
    }

    pub fn add_shot(&self, shot: Shot) {
        self.shots.borrow_mut().push(shot);
    }
}

pub struct Shot {
    pub x: i32,
    pub y: i32,
    pub heading: u32,
}

pub struct Bot {
    pub tank: RefCell<Tank>,
    pub cpu: RefCell<Cpu>,
    pub peripherals: RefCell<BTreeMap<u16, Box<dyn Peripheral>>>,
}

impl Bot {
    pub fn tank_mut(&self) -> std::cell::RefMut<Tank> {
        self.tank.borrow_mut()
    }

    fn tick_cpu(&self) {
        self.cpu.borrow_mut().cycle(self);
    }

    fn tick_peripherals(&self, world: &World) {
        for (_, peripheral) in self.peripherals.borrow_mut().iter_mut() {
            peripheral.tick(self, world);
        }
    }

    pub fn write_peripheral_word(&self, address: u16, value: u16) {
        for (&addr, peripheral) in self.peripherals.borrow_mut().iter_mut() {
            if addr <= address && (addr + peripheral.size()) > address {
                peripheral.write_word(self, address - addr, value);
            }
        }
    }
    pub fn write_peripheral_byte(&self, address: u16, value: u8) {
        for (&addr, peripheral) in self.peripherals.borrow_mut().iter_mut() {
            if addr <= address && (addr + peripheral.size()) > address {
                peripheral.write_byte(self, address - addr, value);
            }
        }
    }
    pub fn read_peripheral_word(&self, address: u16) -> u16 {
        for (&addr, peripheral) in self.peripherals.borrow_mut().iter_mut() {
            if addr <= address && (addr + peripheral.size()) > address {
                return peripheral.read_word(self, address - addr);
            }
        }
        return 0;
    }
    pub fn read_peripheral_byte(&self, address: u16) -> u8 {
        for (&addr, peripheral) in self.peripherals.borrow_mut().iter_mut() {
            if addr <= address && (addr + peripheral.size()) > address {
                return peripheral.read_byte(self, address - addr);
            }
        }
        return 0;
    }
}

#[cfg(not(feature="wasm_ffi"))]
impl From<&Path> for Bot {
    fn from(file: &Path) -> Self {
        let mut file = File::open(file).expect("File not found.");
        let reader: &mut dyn Read = &mut file;
        Self::from(reader)
    }
}

impl From<&mut dyn Read> for Bot {
    fn from(reader: &mut dyn Read) -> Self {
        let mut data: Vec<u8> = vec![];
        reader.read_to_end(&mut data).expect("Read failed");
        Self::from(&data)
    }
}

impl From<&Vec<u8>> for Bot {
    fn from(data: &Vec<u8>) -> Self {
        Self::from(&data[..])
    }
}

impl From<&[u8]> for Bot {
    fn from(data: &[u8]) -> Self {
        let bot = Bot {
            tank: RefCell::new(Tank::default()),
            cpu: RefCell::new(Cpu::default()),
            peripherals: RefCell::new(BTreeMap::new())
        };

        bot.tank.borrow_mut().health = 100;
        {
            let mut cpu = bot.cpu.borrow_mut();
            cpu.user_mem_max = 0xefff;
            cpu.registers[10] = 0xefff;
            for (i, elem) in data.iter().enumerate() {
                cpu.memory[i] = *elem;
            }
        }

        {
            let mut peripherals = bot.peripherals.borrow_mut();
            peripherals.insert(0xfef0, Box::new(ResetPeripheral::default()));
            peripherals.insert(0xfee0, Box::new(RadarPeripheral::default()));
            peripherals.insert(0xfed0, Box::new(TurretPeripheral::default()));
            peripherals.insert(0xfec0, Box::new(HullPeripheral::default()));
        }

        bot
    }
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[cfg_attr(any(feature="c_ffi", feature="wasm_ffi"), repr(C))]
#[derive(Default, Clone)]
pub struct Tank {
    pub x: i32,
    pub y: i32,

    pub heading: u32,
    speed: i32,
    pub turret_offset: u32,
    pub scanner_offset: u32,

    pub health: u8,

    pub _req_steering: u16,
    pub _req_throttle: i16,
    pub _req_turret_steering: u16,
    pub _req_turret_keepshift: u8,
    pub _req_scanner_steering: u16,
    pub _req_scanner_keepshift: u8
}
