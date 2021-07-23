use std::os::raw::c_char;
use std::ffi::CStr;

use crate::world::{World, WorldConfig, Tank, Event};

#[no_mangle]
pub unsafe extern "C" fn bots_default_world_config() -> WorldConfig {
    WorldConfig::default()
}

#[no_mangle]
pub unsafe extern "C" fn bots_world_new(config: &WorldConfig) -> *mut World {
    Box::into_raw(Box::new(World::new(config.clone())))
}

#[no_mangle]
pub unsafe extern "C" fn bots_world_free(world: *mut World) {
    Box::from_raw(world);
}

#[no_mangle]
pub unsafe extern "C" fn bots_world_add_bot(world: *mut World, filename: *const c_char) {
    let filename = CStr::from_ptr(filename);
    (&mut *world).add_bot(filename.to_str().unwrap());
}

#[no_mangle]
pub unsafe extern "C" fn bots_world_place_bots(world: *mut World) {
    (&mut *world).place_bots();
}

#[no_mangle]
pub unsafe extern "C" fn bots_tick(world: *mut World) {
    (&mut *world).tick();
}

#[no_mangle]
pub unsafe extern "C" fn bots_get_tick_events(world: *const World, size: &mut usize) -> *const Event {
    let world = &*world;
    let events = world.events.borrow();
    *size = events.len();
    events.as_ptr()
}

/// docstring test
#[no_mangle]
pub unsafe extern "C" fn bots_get_tank(world: *const World, tank_idx: usize) -> *const Tank {
    &*(&*world).bots[tank_idx].tank.borrow()
}
