#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "custom_fonts.hpp"
#include "math_types.hpp"
#include "button.hpp"
#include "collisions.hpp"

void draw_letter(PixelMap *pixel_map, char letter, int font_size, int font_mag, Vec2i pos, Vec3i color);
void draw_letter_partial(PixelMap *pixel_map, char letter, int font_size, int font_mag, Vec2i pos, Vec3i color, CollisionBox col_box);

void draw_sentence(PixelMap *pixel_map, const char *sentence, int font_size, int font_mag, Vec2i pos, Vec3i color);
void draw_sentence_partial(PixelMap *pixel_map, const char *sentence, int font_size, int font_mag, Vec2i pos, Vec3i color, CollisionBox col_box);

void draw_rectangle(PixelMap *pixel_map, int width, int height, Vec2i pos, Vec3i color);
void draw_rectangle_partial(PixelMap *pixel_map, int width, int height, Vec2i pos, Vec3i color, CollisionBox col_box);

void draw_button(PixelMap *pixel_map, Button *button, bool hover);
void draw_button_partial(PixelMap *pixel_map, Button *button, bool hover, CollisionBox col_box);

void draw_pixmap(PixelMap *dest, PixelMap *src, Vec2i pos);


