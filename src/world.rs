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
        todo!();
    }

    pub fn place_bots(&mut self) {
        todo!();
    }

    pub fn tick(&mut self) {
        todo!();
    }
}

pub struct Bot {
    pub tank: Tank,
    cpu: Cpu
}

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
