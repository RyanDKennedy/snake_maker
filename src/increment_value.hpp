#pragma once

#include "button.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"

struct IncrementValue
{
    Button text;
    Button up;
    Button down;
    int value;
    bool ready;
    int increment_amt;
    bool has_bottom_limit;
    int bottom_limit = 0;
    bool has_top_limit;
    int top_limit = 0;
};

IncrementValue create_increment_value(int initial_value, const char *name, Vec2i pos, Vec3i bg_color, Vec3i fg_color);
void draw_increment_value(PixelMap *pixel_map, IncrementValue *val, Vec2i mouse_pos);
void update_increment_value(IncrementValue *val, Vec2i mouse_pos, bool mouse_clicked, bool mouse_released);
