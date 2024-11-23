#pragma once

#include "math_types.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"

struct TextBox
{
    Vec2i pos;
    int width;
    int height;
    char *text;
    int text_size;

    int font_size;
    int font_mag;
};



TextBox text_box_create(int text_size, Vec2i pos, int font_size, int font_mag);
void text_box_destroy(TextBox *text_box);

void text_box_parse_key_list(TextBox *text_box, char *key_list);

void draw_text_box(PixelMap *pixel_map, TextBox *text_box);
