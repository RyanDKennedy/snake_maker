#include "text_box.hpp"
#include "drawing.hpp"

#include <cstdlib>
#include <cstring>

TextBox text_box_create(int text_size, Vec2i pos, int font_size, int font_mag)
{
    TextBox text_box;

    text_box.width = font_size * font_mag * (text_size + 2);
    text_box.height = font_size * font_mag * 2;
    text_box.text_size = text_size;
    text_box.text = (char*)calloc(text_box.text_size + 1, sizeof(char));
    text_box.font_size = font_size;
    text_box.font_mag = font_mag;
    text_box.pos[0] = pos[0];
    text_box.pos[1] = pos[1];

    return text_box;
}

void text_box_destroy(TextBox *text_box)
{
    free(text_box->text);
}

void text_box_parse_key_list(TextBox *text_box, char *key_list)
{
    // Parse key list into the textbod
    if (strlen(key_list) > 0)
    {
	int text_base = strlen(text_box->text);
	int text_index = 0;
	for (int i = 0; i < strlen(key_list); ++i)
	{
	    // handle backspace NOTE: '\b' is code for backspace since nobody uses bell characters
	    if (key_list[i] == '\b')
	    {
		if (text_base + text_index > 0)
		    --text_index;
	    }
	    else
	    {
		text_box->text[text_base + text_index] = key_list[i];

		if (text_base + text_index < text_box->text_size)
		    ++text_index;

	    }
	}
	text_box->text[text_base + text_index] = '\0';
    }
}

void draw_text_box(PixelMap *pixel_map, TextBox *text_box)
{
    draw_rectangle(pixel_map, text_box->width, text_box->height, text_box->pos, Vec3i{255, 255, 255});

    Vec2i pos;
    pos[0] = text_box->pos[0] + text_box->font_size * text_box->font_mag;
    pos[1] = text_box->pos[1] + text_box->height / 2 - (text_box->font_size * text_box->font_mag / 2);

    draw_sentence(pixel_map, text_box->text, text_box->font_size, text_box->font_mag, pos, Vec3i{0, 0, 0});

    Vec2i cur_pos;
    cur_pos[0] = pos[0] + strlen(text_box->text) * text_box->font_size * text_box->font_mag;
    cur_pos[1] = text_box->pos[1] + text_box->height / 8;

    draw_rectangle(pixel_map, 10, text_box->height * 0.75, cur_pos, Vec3i{50, 50, 50});
}
