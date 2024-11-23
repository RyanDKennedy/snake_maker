#include "tile_create.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "increment_value.hpp"
#include "pixel_map.hpp"
#include "map.hpp"

#include <cstdlib>
#include <cstring>

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx)
{
    TileCreateCtx *ctx = (TileCreateCtx*)calloc(1, sizeof(TileCreateCtx));
    for (int i = 0 ; i < ctx->tile_size; ++i)
    {
	int value = 0;
	ctx->tile[i].r = value;
	ctx->tile[i].g = value;
	ctx->tile[i].b = value;
    }
    ctx->tile_pixmap = pixel_map_create(400, 400);

    ctx->red_slider = slider_create(0, 255, 800, 30, Vec2i{0, 90});
    ctx->green_slider = slider_create(0, 255, 800, 30, Vec2i{0, 50});
    ctx->blue_slider = slider_create(0, 255, 800, 30, Vec2i{0, 10});

    ctx->current_color.r = ctx->red_slider.value;
    ctx->current_color.g = ctx->green_slider.value;
    ctx->current_color.b = ctx->blue_slider.value;

    for (int i = 0; i < ctx->old_colors_amt; ++i)
    {
	ctx->old_colors[i].r = 0;
	ctx->old_colors[i].g = 0;
	ctx->old_colors[i].b = 0;
    }

    return ctx;
}

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

/*
    if (generic_ctx->keyboard.space)
    {
	write_tile_to_file("../share/tiles/new.tile", tile_create_ctx->tile, tile_create_ctx->tile_width, tile_create_ctx->tile_height);
    }
*/

    Vec2i tile_pos = {(pixel_map->width - tile_create_ctx->tile_pixmap.width) / 2, (pixel_map->height - tile_create_ctx->tile_pixmap.height) / 2};
    int pixel_width = tile_create_ctx->tile_pixmap.width / tile_create_ctx->tile_width;
    int pixel_height = tile_create_ctx->tile_pixmap.height / tile_create_ctx->tile_height;

    Vec2i old_colors_pos = {0, 300};
    const int old_color_pixel_width = 30;
    const int old_color_pixel_height = 30;

    // see if old colors is clicked
    {
	if (generic_ctx->mouse_clicked && generic_ctx->mouse_pos[0] >= old_colors_pos[0] && generic_ctx->mouse_pos[0] < old_colors_pos[0] + old_color_pixel_width && generic_ctx->mouse_pos[1] >= old_colors_pos[1] && generic_ctx->mouse_pos[1] < old_colors_pos[1] + tile_create_ctx->old_colors_amt * old_color_pixel_height)
	{
	    int y = (generic_ctx->mouse_pos[1] - old_colors_pos[1]) / old_color_pixel_height;	    

	    tile_create_ctx->red_slider.value = tile_create_ctx->old_colors[y].r;
	    tile_create_ctx->green_slider.value = tile_create_ctx->old_colors[y].g;
	    tile_create_ctx->blue_slider.value = tile_create_ctx->old_colors[y].b;

	}
    }

    // update sliders
    slider_update(&tile_create_ctx->red_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->green_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->blue_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);

    // update current color
    tile_create_ctx->current_color.r = tile_create_ctx->red_slider.value;
    tile_create_ctx->current_color.g = tile_create_ctx->green_slider.value;
    tile_create_ctx->current_color.b = tile_create_ctx->blue_slider.value;

    // Updating tile
    if (generic_ctx->mouse_clicked)
    {
	int x = (generic_ctx->mouse_pos[0] - tile_pos[0]) / pixel_width;
	int y = (generic_ctx->mouse_pos[1] - tile_pos[1]) / pixel_height;

	if (x >= 0 && x < tile_create_ctx->tile_width &&
	    y >= 0 && y < tile_create_ctx->tile_height)
	{
	    tile_create_ctx->tile[x + y * tile_create_ctx->tile_width] = tile_create_ctx->current_color;

	    // See if color is already in old colors
	    int index_of_old_color = tile_create_ctx->old_colors_amt - 1;
	    for (int i = 0; i < tile_create_ctx->old_colors_amt; ++i)
	    {
		RGBPixel cur = tile_create_ctx->current_color;
		RGBPixel now = tile_create_ctx->old_colors[i];

		if (cur.r == now.r && cur.g == now.g && cur.b == now.b)
		{
		    index_of_old_color = i;
		    break;
		}
	    }

	    // Actually put color into old colors
	    for (int i = index_of_old_color; i > 0; --i)
	    {
		tile_create_ctx->old_colors[i] = tile_create_ctx->old_colors[i - 1];
	    }
	    tile_create_ctx->old_colors[0] = tile_create_ctx->current_color;
	}

    }

    // Drawing old colors
    {
	Vec3i bg = {50, 50, 50};
	const int border_size = 3;
	for (int i = 0; i < tile_create_ctx->old_colors_amt; ++i)
	{
	    Vec3i col;
	    col[0] = tile_create_ctx->old_colors[i].r;
	    col[1] = tile_create_ctx->old_colors[i].g;
	    col[2] = tile_create_ctx->old_colors[i].b;

	    Vec2i border_pos;
	    border_pos[0] = old_colors_pos[0];
	    border_pos[1] = old_colors_pos[1] + i * old_color_pixel_height;

	    Vec2i pos;
	    pos[0] = border_pos[0] + border_size;
	    pos[1] = border_pos[1] + border_size;

	    draw_rectangle(pixel_map, 30, 30, border_pos, bg);
	    draw_rectangle(pixel_map, old_color_pixel_width - border_size * 2, old_color_pixel_height - border_size * 2, pos, col);
	}
    }

    // Drawing current color
    {
	Vec3i color;
	color[0] = tile_create_ctx->current_color.r;
	color[1] = tile_create_ctx->current_color.g;
	color[2] = tile_create_ctx->current_color.b;
	draw_rectangle(pixel_map, pixel_map->width, 30, Vec2i{0, 130}, color);

	char buf[256];
	snprintf(buf, 256, "# %02x %02x %02x", color[0], color[1], color[2]);

	draw_sentence(pixel_map, buf, 8, 2, Vec2i{0, 170}, Vec3i{255, 255, 255});

    }

    // draw sliders
    draw_slider(pixel_map, &tile_create_ctx->red_slider, Vec3i{255, 0, 0});
    draw_slider(pixel_map, &tile_create_ctx->green_slider, Vec3i{0, 255, 0});
    draw_slider(pixel_map, &tile_create_ctx->blue_slider, Vec3i{0, 0, 255});

    // Drawing to pixmap
    for (int y = 0; y < tile_create_ctx->tile_height; ++y)
    {
	int y_offset = y * tile_create_ctx->tile_width;
	for (int x = 0; x < tile_create_ctx->tile_width; ++x)
	{
	    int index = y_offset + x;
	    Vec3i color;
	    color[0] = tile_create_ctx->tile[index].r;
	    color[1] = tile_create_ctx->tile[index].g;
	    color[2] = tile_create_ctx->tile[index].b;

	    draw_rectangle(&tile_create_ctx->tile_pixmap, pixel_width, pixel_height, Vec2i{x * pixel_width, y *pixel_height}, color);
	}
    }    

    // Drawing tile
    int border_size = 20;
    draw_rectangle(pixel_map, tile_create_ctx->tile_pixmap.width + border_size * 2, tile_create_ctx->tile_pixmap.height + border_size * 2, Vec2i{tile_pos[0] - border_size, tile_pos[1] - border_size}, Vec3i{50, 50, 50});
    border_size = 5;
    draw_rectangle(pixel_map, tile_create_ctx->tile_pixmap.width + border_size * 2, tile_create_ctx->tile_pixmap.height + border_size * 2, Vec2i{tile_pos[0] - border_size, tile_pos[1] - border_size}, Vec3i{0, 0, 0});
    draw_pixmap(pixel_map, &tile_create_ctx->tile_pixmap, tile_pos);
    
    return return_code;
}

void tile_create_end(GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    pixel_map_destroy(&tile_create_ctx->tile_pixmap);
    free(tile_create_ctx);
}
