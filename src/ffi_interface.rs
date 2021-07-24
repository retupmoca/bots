/**
 * NOTE: Everything in this file should be considered unsafe!
 *
 * Due to limitations of wasm_bindgen, the functions themselves can't be marked
 * unsafe, but if you treat these like safe functions, you get to keep both
 * pieces.
 */

#[cfg(feature = "c_ffi")]
use std::os::raw::c_char;
#[cfg(feature = "c_ffi")]
use std::ffi::CStr;
#[cfg(feature = "c_ffi")]
use std::path::Path;

#[cfg(feature = "wasm_ffi")]
use wasm_bindgen::prelude::*;

use crate::world::{World, WorldConfig, Tank, Event};

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_default_world_config() -> WorldConfig {
    WorldConfig::default()
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_world_new(config: &WorldConfig) -> *mut World {
    Box::into_raw(Box::new(World::new(config.clone())))
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_world_free(world: *mut World) {
    unsafe {
        Box::from_raw(world);
    }
}

#[cfg(feature = "c_ffi")]
#[no_mangle]
pub unsafe extern "C" fn bots_world_add_bot(world: *mut World, filename: *const c_char) {
    let filename = CStr::from_ptr(filename);
    let filename = filename.to_string_lossy();
    (&mut *world).add_bot(Path::new(&*filename));
}

#[cfg(feature = "c_ffi")]
#[no_mangle]
pub unsafe extern "C" fn bots_world_add_bot_from_buf(world: *mut World, buf: *const u8, sz: usize) {
    let data = std::slice::from_raw_parts(buf, sz);
    (&mut *world).add_bot_from_data(data)
}

#[cfg(feature="wasm_ffi")]
#[wasm_bindgen]
pub fn bots_world_add_bot_from_data(world: *mut World, buf: &[u8]) {
    unsafe {
        (&mut *world).add_bot_from_data(buf)
    }
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_world_place_bots(world: *mut World) {
    unsafe {
        (&mut *world).place_bots();
    }
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_tick(world: *mut World) {
    unsafe {
        (&mut *world).tick();
    }
}

// TODO: wasm
#[no_mangle]
pub extern "C" fn bots_get_tick_events(world: *const World, size: &mut usize) -> *const Event {
    unsafe {
        let world = &*world;
        let events = world.events.borrow();
        *size = events.len();
        events.as_ptr()
    }
}

#[cfg(feature="c_ffi")]
#[no_mangle]
pub extern "C" fn bots_get_tank(world: *const World, tank_idx: usize) -> *const Tank {
    unsafe {
        &*(&*world).bots[tank_idx].tank.borrow()
    }
}

#[cfg_attr(feature = "wasm_ffi", wasm_bindgen)]
#[no_mangle]
pub extern "C" fn bots_get_tank_value(world: *const World, tank_idx: usize) -> Tank {
    unsafe {
        (*(&*world).bots[tank_idx].tank.borrow()).clone()
    }
}
