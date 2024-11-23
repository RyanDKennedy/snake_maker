#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"
#include "math_types.hpp"

struct Slider
{
    int min_value;
    int max_value;
    int value;
    int width;
    int height;
    Vec2i pos;
    int selector_size;
};

Slider slider_create(int min, int max, int width, int height, Vec2i pos);

void slider_update(Slider *slider, Vec2i mouse_pos, bool mouse_clicked);

void draw_slider(PixelMap* pixel_map, Slider *slider, Vec3i color);
