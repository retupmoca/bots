#include <bots/struct.h>

void bots_phy_push(
    bots_world *world, bots_world_object *obj,
    uint16_t obj_offset_x, uint16_t obj_offset_y,
    uint16_t force_angle, int32_t force_value
) {
    //// scale force by object mass
    //
    //// break force into "rotate" and "push" components
    // get angle to CoM: arctan(offset_x/offset_y)
    // get components:
    //  Special-case: exact 90 degree (0.5pi) multiple, just use value directly
    //  sin(difference_angle)*force_value == rotation force
    //  cos(difference_angle)*force_value == push force
    //
    //// scale rotation force by lever action
    // rotation force *= distance from CoM
    //// accumulate rotation rate into object
    // object rotation velocity += rotation force
    //
    //// rotate push vector into object orientation
    // CoM angle += object rotation
    //// accumulate push vector into object
    // object velocity += push force
}


