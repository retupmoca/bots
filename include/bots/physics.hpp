#pragma once

namespace bots {
namespace physics {

class Vec {
    public:
    uint16_t angle;
    uint16_t magnitude;
};

Vec operator+(Vec, Vec);

class Object {
    uint32_t x;
    uint32_t y;
    uint16_t rotation;

    Vec velocity;

    uint16_t rotation_speed;

    void push(uint16_t obj_offset_x, uint16_t obj_offset_y, Vec force);
};

class RectObject : public Object {
    uint16_t width;
    uint16_t height;
};

class ArcObject : public Object {
    uint16_t arc;
    uint16_t outer_radius;
    uint16_t inner_radius;
};

}
}
