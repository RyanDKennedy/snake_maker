#pragma once

#include "math_types.hpp"

struct CollisionBox
{
    Vec2i bottom_left;
    Vec2i top_right;
};

bool is_inside_collision_box(CollisionBox *box, Vec2i pos);
