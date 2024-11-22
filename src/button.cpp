#include "button.hpp"

#include <cstring>

Button create_button(const char *text, int width, int height, Vec2i pos, Vec3i bg_color, Vec3i fg_color, int font_size, int font_mag)
{
    Button result;
    result.text = text;
    result.width = width;
    result.height = height;
    result.col_box.bottom_left[0] = pos[0];
    result.col_box.bottom_left[1] = pos[1];
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

void move_button(Button *button, Vec2i pos, bool translate)
{
    // Set pos according to the translate value
    if (translate)
    {
	pos[0] += button->col_box.bottom_left[0];
	pos[1] += button->col_box.bottom_left[1];
    }

    memcpy(button->col_box.bottom_left, pos, sizeof(Vec2i));
    memcpy(button->col_box.top_right, Vec2i{pos[0] + button->width, pos[1] + button->height}, sizeof(Vec2i));
}
