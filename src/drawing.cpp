#include "drawing.hpp"

#include <cstdlib>
#include <cassert>
#include <cstring>

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
    for (int i = 0; i < strlen(sentence); ++i)
    {
	draw_letter(pixel_map, sentence[i], font_size, font_mag, pos, color);
	pos[0] += (font_size * font_mag) + 1;
    }
}

void draw_rectangle(PixelMap *pixel_map, int width, int height, Vec2i pos, Vec3i color)
{
    for (int y = pos[1]; y < pos[1] + height; ++y)
    {
	for (int x = pos[0]; x < pos[0] + width; ++x)
	{
	    pixel_map->data[y * pixel_map->width + x].r = color[0];
	    pixel_map->data[y * pixel_map->width + x].g = color[1];
	    pixel_map->data[y * pixel_map->width + x].b = color[2];
	}
    }

}
