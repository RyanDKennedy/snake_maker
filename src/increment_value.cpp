#include "increment_value.hpp"

#include <cstring>

IncrementValue create_increment_value(int initial_value, const char *name, Vec2i pos, Vec3i bg_color, Vec3i fg_color)
{
    IncrementValue value;

    const int font_size = 8;
    const int font_mag = 2;
    const int text_width = font_size * font_mag * (strlen(name) + 1);

    const int height = font_size * font_mag * 2;

    Vec2i down_pos;
    down_pos[0] = pos[0] + text_width;
    down_pos[1] = pos[1];

    Vec2i up_pos;
    up_pos[0] = down_pos[0] + font_size * font_mag * 2;
    up_pos[1] = down_pos[1];

    value.value = initial_value;
    value.text = create_button(name, text_width, height, pos, bg_color, fg_color, font_size, font_mag);
    value.down = create_button("v", font_size * font_mag * 2, height, down_pos, bg_color, fg_color, font_size, font_mag);
    memcpy(&value.down.fg_hover_color, bg_color, sizeof(Vec3i));
    memcpy(&value.down.bg_hover_color, fg_color, sizeof(Vec3i));

    value.up = create_button("^", font_size * font_mag * 2, height, up_pos, bg_color, fg_color, font_size, font_mag);
    memcpy(&value.up.fg_hover_color, bg_color, sizeof(Vec3i));
    memcpy(&value.up.bg_hover_color, fg_color, sizeof(Vec3i));

    value.ready = true;

    return value;
}

void draw_increment_value(PixelMap *pixel_map, IncrementValue *val, Vec2i mouse_pos)
{
    bool up_hover = is_inside_collision_box(&val->up.col_box, mouse_pos);
    bool down_hover = is_inside_collision_box(&val->down.col_box, mouse_pos);

    draw_button(pixel_map, &val->text, false);
    draw_button(pixel_map, &val->down, down_hover);
    draw_button(pixel_map, &val->up, up_hover);

    Vec2i val_pos;
    val_pos[0] = val->up.col_box.top_right[0];
    val_pos[1] = val->up.col_box.bottom_left[1];

    char buf[256];
    snprintf(buf, 256, " %d", val->value);
    draw_sentence(pixel_map, buf, 8, 3, val_pos, Vec3i{255, 255, 255});
}

void update_increment_value(IncrementValue *val, Vec2i mouse_pos, bool mouse_clicked, bool mouse_released)
{
    if (mouse_released == true)
    {
	val->ready = true;
    }

    if (mouse_clicked && val->ready)
    {
	bool up_hover = is_inside_collision_box(&val->up.col_box, mouse_pos);
	bool down_hover = is_inside_collision_box(&val->down.col_box, mouse_pos);

	if (up_hover)
	    ++val->value;
	
	if (down_hover && val->value > 0)
	    --val->value;

	val->ready = false;
    }
}






