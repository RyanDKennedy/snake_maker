#pragma once

#include "collisions.hpp"
#include "math_types.hpp"

struct Button
{
    CollisionBox col_box;
    Vec3i bg_color;
    Vec3i bg_hover_color;
    Vec3i fg_color;
    Vec3i fg_hover_color;
    int width;
    int height;
    const char *text;
    int font_size;
    int font_mag;

};

Button create_button(const char *text, int width, int height, Vec2i pos, Vec3i bg_color, Vec3i fg_color, int font_size, int font_mag);
