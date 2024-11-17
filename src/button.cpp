#include "button.hpp"

#include <cstring>

Button create_button(const char *text, int width, int height, Vec2i pos, Vec3i bg_color, Vec3i fg_color, int font_size, int font_mag)
{
    Button result;
    result.text = text;
    result.width = width;
    result.height = height;
    memcpy(result.col_box.bottom_left, pos, sizeof(Vec2i));
    result.col_box.top_right[0] = result.col_box.bottom_left[0] + width;
    result.col_box.top_right[1] = result.col_box.bottom_left[1] + height;

    result.font_size = font_size;
    result.font_mag = font_mag;

    memcpy(result.bg_color, bg_color, sizeof(Vec3i));
    memcpy(result.bg_hover_color, bg_color, sizeof(Vec3i));
    memcpy(result.fg_color, fg_color, sizeof(Vec3i));
    memcpy(result.fg_hover_color, fg_color, sizeof(Vec3i));

    return result;
}
