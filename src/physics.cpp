#include <cstdint>
#include <cmath>
#include <bots/physics.hpp>

namespace bots {
namespace physics {

Vec operator+(Vec a, Vec b) {
    return {};
}

void Object::push(uint16_t obj_offset_x, uint16_t obj_offset_y, Vec force) {
    //// scale force by object mass
    //
    //// break force into "rotate" and "push" components
    // get angle to CoM: arctan(offset_x/offset_y)
    double angle_to_com = atan(obj_offset_x/obj_offset_y);
    // get components:
    //  Special-case: exact 90 degree (0.5pi) multiple, just use value directly
    //  sin(difference_angle)*force_value == rotation force
    double difference_angle = force.angle - angle_to_com;
    double rot_force = sin(difference_angle) * force.magnitude;
    //  cos(difference_angle)*force_value == push force
    double push_force = cos(difference_angle) * force.magnitude;
    //
    //// scale rotation force by lever action
    // rotation force *= distance from CoM
    rot_force *= pow(obj_offset_x * obj_offset_x + obj_offset_y * obj_offset_y, 0.5);
    //// accumulate rotation rate into object
    // object rotation velocity += rotation force
    rotation_speed += rot_force;
    //
    //// rotate push vector into object orientation
    // CoM angle += object rotation
    //// accumulate push vector into object
    // object velocity += push force
    velocity += Vec(angle_to_com, push_force);
}

}
}
