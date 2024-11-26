#include "slider.hpp"

#include "collisions.hpp"

Slider slider_create(int min, int max, int width, int height, Vec2i pos)
{
    Slider slider;

    slider.min_value = min;
    slider.max_value = max;
    slider.width = width;
    slider.height = height;
    slider.pos[0] = pos[0];
    slider.pos[1] = pos[1];
    slider.selector_size = slider.height / 2;
    slider.value = min;

    return slider;
}

void slider_update(Slider *slider, Vec2i mouse_pos, bool mouse_clicked)
{
    CollisionBox col_box;
    col_box.bottom_left[0] = slider->pos[0] + slider->selector_size /2;
    col_box.top_right[0] = slider->pos[0] + slider->width - slider->selector_size / 2;
    col_box.bottom_left[1] = slider->pos[1];
    col_box.top_right[1] = slider->pos[1] + slider->height;

    if (is_inside_collision_box(&col_box, mouse_pos) && mouse_clicked)
    {
	int range = slider->max_value - slider->min_value;
	float percent = (float)(mouse_pos[0] - col_box.bottom_left[0]) / (col_box.top_right[0] - col_box.bottom_left[0]);

	slider->value = percent * range + slider->min_value;
    }
}

void draw_slider(PixelMap* pixel_map, Slider *slider, Vec3i color)
{
    // draw slider bar
    Vec2i bar_pos;
    bar_pos[0] = slider->pos[0] + slider->selector_size / 2;
    bar_pos[1] = slider->pos[1] + slider->height / 2 - slider->height / 4;
    draw_rectangle(pixel_map, slider->width - slider->selector_size, slider->height / 2, bar_pos, color, 255);
    
    // draw selector
    CollisionBox col_box;
    col_box.bottom_left[0] = slider->pos[0] + slider->selector_size / 2;
    col_box.top_right[0] = slider->pos[0] + slider->width - slider->selector_size / 2;
    col_box.bottom_left[1] = slider->pos[1];
    col_box.top_right[1] = slider->pos[1] + slider->height;

    double percent = (double)(slider->value - slider->min_value) / (slider->max_value - slider->min_value);

    Vec2i slider_pos;
    slider_pos[0] = (int)(percent * (col_box.top_right[0] - col_box.bottom_left[0])) + slider->pos[0];
    slider_pos[1] = slider->pos[1];
    draw_rectangle(pixel_map, slider->selector_size, slider->height, slider_pos, Vec3i{255, 255, 255}, 255);
}
