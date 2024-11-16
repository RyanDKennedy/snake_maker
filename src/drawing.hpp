#pragma once

#include "pixel_map.hpp"
#include "custom_fonts.hpp"
#include "math_types.hpp"


void draw_letter(PixelMap *pixel_map, char letter, int font_size, int font_mag, Vec2i pos, Vec3i color);

void draw_sentence(PixelMap *pixel_map, const char *sentence, int font_size, int font_mag, Vec2i pos, Vec3i color);

void draw_rectangle(PixelMap *pixel_map, int width, int height, Vec2i pos, Vec3i color);


