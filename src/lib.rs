#[cfg(feature="wasm_ffi")]
use wee_alloc::WeeAlloc;
#[cfg(feature="wasm_ffi")]
#[global_allocator]
static ALLOC: WeeAlloc = WeeAlloc::INIT;

mod world;
mod cpu;
mod ops;
mod peripherals;

#[cfg(any(feature="c_ffi", feature="wasm_ffi"))]
pub mod ffi_interface;

pub use self::world::{World, WorldConfig};
