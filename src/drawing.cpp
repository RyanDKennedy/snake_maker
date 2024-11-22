#include "drawing.hpp"
#include "pixel.hpp"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>

void draw_letter(PixelMap *pixel_map, char letter, int font_size, int font_mag, Vec2i pos, Vec3i color)
{
    assert(font_size == 8 || font_size == 16);
    assert(font_mag > 0);

    const char *font_bitmap = NULL;

    switch(font_size)
    {
	case 8:
	    font_bitmap = g_eightbitfont[letter];
	    break;
    }

    if (font_mag != 1)
    {
	// prepare letter
	const int new_size = font_size * font_mag;
	char *bitmap = (char*)calloc(new_size * new_size, sizeof(char));
	memset(bitmap, ' ', new_size * new_size);
	
	for (int y = 0; y < font_size; ++y)
	{
	    for (int x = 0; x < font_size; ++x)
	    {
		if (font_bitmap[y * font_size + x] == '1')
		{
		    int new_y = y * 2;
		    int new_x = x * 2;
		    bitmap[new_y * new_size + new_x] = '1';
		    bitmap[new_y * new_size + new_x + 1] = '1';
		    bitmap[(new_y + 1) * new_size + new_x] = '1';
		    bitmap[(new_y + 1) * new_size + new_x + 1] = '1';
		}
	    }
	}

	font_bitmap = bitmap;
	font_size = new_size;
    }

    if (font_bitmap == NULL)
    {
	font_bitmap = g_eightbitfont[' '];
    }

    for (int h = font_size-1; h >= 0; h--)
    {
	for (int w = 0; w < font_size; ++w)
	{
	    if (font_bitmap[h * font_size + w] == '1')
	    {
#ifndef NDEBUG
		const int x = pos[0] + w;
		const int y = (font_size - h) + pos[1];
		if (x < 0 || x >= pixel_map->width || y < 0 || y >= pixel_map->height)
		    continue;
#endif
		
		int base = pos[0] + pos[1] * pixel_map->width;
		const int index = (font_size - h) * pixel_map->width + w + base;
		pixel_map->data[index].r = color[0];
		pixel_map->data[index].g = color[1];
		pixel_map->data[index].b = color[2];
	    }
	}
    }

    if (font_mag != 1)
    {
	free((char*)font_bitmap);
    }

}

void draw_sentence(PixelMap *pixel_map, const char *sentence, int font_size, int font_mag, Vec2i pos, Vec3i color)
{
    int xval = pos[0];
    for (int i = 0; i < strlen(sentence); ++i)
    {
	draw_letter(pixel_map, sentence[i], font_size, font_mag, Vec2i{xval, pos[1]}, color);
	xval += (font_size * font_mag);
    }
}

void draw_rectangle(PixelMap *pixel_map, int width, int height, Vec2i pos, Vec3i color)
{
    for (int y = pos[1]; y < pos[1] + height; ++y)
    {
	for (int x = pos[0]; x < pos[0] + width; ++x)
	{
#ifndef NDEBUG
	    if (x < 0 || x >= pixel_map->width || y < 0 || y >= pixel_map->height)
		continue;
#endif

	    pixel_map->data[y * pixel_map->width + x].r = color[0];
	    pixel_map->data[y * pixel_map->width + x].g = color[1];
	    pixel_map->data[y * pixel_map->width + x].b = color[2];
	}
    }

}

void draw_button(PixelMap *pixel_map, Button *button, bool hover)
{
    Vec3i bg;
    Vec3i fg;
    if (hover)
    {
	memcpy(bg, button->bg_hover_color, sizeof(Vec3i));
	memcpy(fg, button->fg_hover_color, sizeof(Vec3i));
    }
    else
    {
	memcpy(bg, button->bg_color, sizeof(Vec3i));
	memcpy(fg, button->fg_color, sizeof(Vec3i));
    }

    // truncate text
    char text[255];
    const int char_size = button->font_size * button->font_mag;
    snprintf(text, button->width / char_size + 1, "%s", button->text);

    Vec2i text_pos;
    text_pos[0] = button->col_box.bottom_left[0] + (button->width / 2) - (strlen(text) * char_size / 2);
    text_pos[1] = button->col_box.bottom_left[1] + (button->height / 2) - (char_size / 2);

    draw_rectangle(pixel_map, button->width, button->height, button->col_box.bottom_left, bg);
    draw_sentence(pixel_map, text, button->font_size, button->font_mag, text_pos, fg);
}

void draw_pixmap(PixelMap *dest, PixelMap *src, Vec2i pos)
{
    for (int y = 0; y < src->height; ++y)
    {
	for (int x = 0; x < src->width; ++x)
	{

	    Vec2i dest_pos = {pos[0] + x, pos[1] + y};

#ifndef NDEBUG
	    if (dest_pos[0] < 0 || dest_pos[0] >= dest->width || dest_pos[1] < 0 || dest_pos[1] >= dest->height)
		continue;
#endif
	    
	    memcpy(&dest->data[dest_pos[0] + dest_pos[1] * dest->width], &src->data[x + y * src->width], sizeof(RGBPixel));
	}

    }


}


