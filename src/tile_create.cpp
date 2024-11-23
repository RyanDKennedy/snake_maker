#include "tile_create.hpp"
#include "game_state.hpp"
#include "increment_value.hpp"
#include "pixel_map.hpp"

#include <cstdlib>
#include <cstring>

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx)
{
    TileCreateCtx *ctx = (TileCreateCtx*)calloc(1, sizeof(TileCreateCtx));
    for (int i = 0 ; i < ctx->tile_size; ++i)
    {
//	int value = (int)(255.0 / (float)ctx->tile_size * (float)i);
	int value = 0;
	ctx->tile[i].r = value;
	ctx->tile[i].g = value;
	ctx->tile[i].b = value;
    }
    ctx->tile_pixmap = pixel_map_create(400, 400);

/*
    ctx->red_value = create_increment_value(0, "red", Vec2i{0, 0}, Vec3i{0, 0, 0}, Vec3i{255, 0, 0});
    ctx->red_value.has_bottom_limit = true;
    ctx->red_value.bottom_limit = 0;
    ctx->red_value.has_top_limit = true;
    ctx->red_value.top_limit = 255;
    ctx->red_value.increment_amt = 10;

    ctx->green_value = create_increment_value(0, "green", Vec2i{0, 32}, Vec3i{0, 0, 0}, Vec3i{0, 255, 0});
    ctx->green_value.has_bottom_limit = true;
    ctx->green_value.bottom_limit = 0;
    ctx->green_value.has_top_limit = true;
    ctx->green_value.top_limit = 255;
    ctx->green_value.increment_amt = 10;

    ctx->blue_value = create_increment_value(0, "blue", Vec2i{0, 64}, Vec3i{0, 0, 0}, Vec3i{0, 0, 255});
    ctx->blue_value.has_bottom_limit = true;
    ctx->blue_value.bottom_limit = 0;
    ctx->blue_value.has_top_limit = true;
    ctx->blue_value.top_limit = 255;
    ctx->blue_value.increment_amt = 10;
*/

//    ctx->current_color = {(uint8_t)ctx->red_value.value, (uint8_t)ctx->green_value.value, (uint8_t)ctx->blue_value.value};

    ctx->red_slider = slider_create(0, 255, 800, 30, Vec2i{0, 90});
    ctx->green_slider = slider_create(0, 255, 800, 30, Vec2i{0, 50});
    ctx->blue_slider = slider_create(0, 255, 800, 30, Vec2i{0, 10});

    return ctx;
}

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

    Vec2i tile_pos = {(pixel_map->width - tile_create_ctx->tile_pixmap.width) / 2, (pixel_map->height - tile_create_ctx->tile_pixmap.height) / 2};
    int pixel_width = tile_create_ctx->tile_pixmap.width / tile_create_ctx->tile_width;
    int pixel_height = tile_create_ctx->tile_pixmap.height / tile_create_ctx->tile_height;

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
