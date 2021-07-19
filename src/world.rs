use std::path::Path;
use std::fs::File;
use std::io::Read;
use std::f64::consts::PI;

use crate::cpu::Cpu;

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

pub struct World {
    config: WorldConfig,
    pub bots: Vec<Bot>
}

impl World {
    pub fn new(config: WorldConfig) -> Self {
        World {
            config,
            bots: vec![]
        }
    }

    pub fn add_bot(&mut self, filename: &str) {
        self.bots.push(Bot::from(Path::new(filename)));
    }

    pub fn place_bots(&mut self) {
        let dist = self.config.spawn_distance;

        let mut angle = 0f64;
        let step = (2f64 * PI) / self.bots.len() as f64;

        for bot in &mut self.bots {
            let loc_x = (dist as f64 * angle.cos() + 0.5).floor() as i32;
            let loc_y = (dist as f64 * angle.sin() + 0.5).floor() as i32;
            bot.tank.x = loc_x;
            bot.tank.y = loc_y;
            angle += step;
        }
    }

    pub fn tick(&mut self) {
        todo!();
    }
}

pub struct Bot {
    pub tank: Tank,
    cpu: Cpu
}

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
        let mut bot = Bot {
            tank: Tank::default(),
            cpu: Cpu::default()
        };

        bot.tank.health = 100;
        bot.cpu.user_mem_max = 0xefff;
        bot.cpu.registers[10] = 0xefff;
        for (i, elem) in data.iter().enumerate() {
            bot.cpu.memory[i] = *elem;
        }

        // TODO: mount peripherals

        bot
    }
}

#[derive(Default)]
pub struct Tank {
    pub x: i32,
    pub y: i32,

    pub heading: u32,
    speed: i32,
    turret_offset: u32,
    scanner_offset: u32,

    health: u8,

    _req_steering: u16,
    _req_throttle: i16,
    _req_turret_steering: u16,
    _req_turret_keepshift: u8,
    _req_scanner_steering: u16,
    _req_scanner_keepshift: u8
}
