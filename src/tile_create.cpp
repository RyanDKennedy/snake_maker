#include "tile_create.hpp"
#include "collisions.hpp"
#include "common.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "pixel_map.hpp"
#include "map.hpp"
#include "text_box.hpp"

#include <cstdlib>
#include <cstring>

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx)
{
    TileCreateCtx *ctx = (TileCreateCtx*)calloc(1, sizeof(TileCreateCtx));
    ctx->tile_width = generic_ctx->settings.tile_create_width;
    ctx->tile_height = generic_ctx->settings.tile_create_height;
    ctx->tile_size = ctx->tile_width * ctx->tile_height;
    ctx->tile = (RGBAPixel*)calloc(ctx->tile_size, sizeof(RGBAPixel));

    for (int i = 0 ; i < ctx->tile_size; ++i)
    {
	int value = (int)(255.0 / (float)ctx->tile_size * (float)i);
	ctx->tile[i].r = value;
	ctx->tile[i].g = value;
	ctx->tile[i].b = value;
	ctx->tile[i].a = 255;
    }
    ctx->tile_pixmap = pixel_map_create(400, 400);

    ctx->red_slider = slider_create(0, 255, 600, 30, Vec2i{100, 90});
    ctx->green_slider = slider_create(0, 255, 600, 30, Vec2i{100, 50});
    ctx->blue_slider = slider_create(0, 255, 600, 30, Vec2i{100, 10});
    ctx->alpha_slider = slider_create(0, 255, 600, 30, Vec2i{100, 650});
    ctx->alpha_slider.value = 255;

    ctx->current_color.r = ctx->red_slider.value;
    ctx->current_color.g = ctx->green_slider.value;
    ctx->current_color.b = ctx->blue_slider.value;
    ctx->current_color.a = ctx->alpha_slider.value;

    for (int i = 0; i < ctx->old_colors_amt; ++i)
    {
	ctx->old_colors[i].r = 0;
	ctx->old_colors[i].g = 0;
	ctx->old_colors[i].b = 0;
	ctx->old_colors[i].a = 255;
    }

    ctx->text = text_box_create(25, Vec2i{150, 700}, 8, 2);
    strcpy(ctx->text.text, "new");

    ctx->save_btn = create_button("save", 100, 32, Vec2i{590, 700}, Vec3i{50, 50, 50}, Vec3i{255, 255, 0}, 8, 2)
;
    memcpy(&ctx->save_btn.bg_hover_color, &ctx->save_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->save_btn.fg_hover_color, &ctx->save_btn.bg_color, sizeof(Vec3i));

    ctx->load_btn = create_button("load", 100, 32, Vec2i{690, 700}, Vec3i{50, 50, 50}, Vec3i{0, 200, 0}, 8, 2);
    memcpy(&ctx->load_btn.bg_hover_color, &ctx->load_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->load_btn.fg_hover_color, &ctx->load_btn.bg_color, sizeof(Vec3i));

    return ctx;
}

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }



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
	    tile_create_ctx->alpha_slider.value = tile_create_ctx->old_colors[y].a;
	}
    }

    // See if right clicked tile
    if (generic_ctx->mouse_right_clicked)
    {
	int x = (generic_ctx->mouse_pos[0] - tile_pos[0]) / pixel_width;
	int y = (generic_ctx->mouse_pos[1] - tile_pos[1]) / pixel_height;

	if (x >= 0 && x < tile_create_ctx->tile_width &&
	    y >= 0 && y < tile_create_ctx->tile_height)
	{
	    const int index = x + y * tile_create_ctx->tile_width;
	    tile_create_ctx->red_slider.value = tile_create_ctx->tile[index].r;
	    tile_create_ctx->green_slider.value = tile_create_ctx->tile[index].g;
	    tile_create_ctx->blue_slider.value = tile_create_ctx->tile[index].b;
	    tile_create_ctx->alpha_slider.value = tile_create_ctx->tile[index].a;
	}
    }

    // update sliders
    slider_update(&tile_create_ctx->red_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->green_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->blue_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->alpha_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);

    // update current color
    tile_create_ctx->current_color.r = tile_create_ctx->red_slider.value;
    tile_create_ctx->current_color.g = tile_create_ctx->green_slider.value;
    tile_create_ctx->current_color.b = tile_create_ctx->blue_slider.value;
    tile_create_ctx->current_color.a = tile_create_ctx->alpha_slider.value;

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
		RGBAPixel cur = tile_create_ctx->current_color;
		RGBAPixel now = tile_create_ctx->old_colors[i];

		if (cur.r == now.r && cur.g == now.g && cur.b == now.b && cur.a == now.a)
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
	snprintf(buf, 256, "# %02x%02x%02x %02x", color[0], color[1], color[2], tile_create_ctx->current_color.a);

	draw_sentence(pixel_map, buf, 8, 2, Vec2i{0, 170}, Vec3i{255, 255, 255});

    }

    // draw sliders
    draw_slider(pixel_map, &tile_create_ctx->red_slider, Vec3i{255, 0, 0});
    draw_slider(pixel_map, &tile_create_ctx->green_slider, Vec3i{0, 255, 0});
    draw_slider(pixel_map, &tile_create_ctx->blue_slider, Vec3i{0, 0, 255});
    draw_slider(pixel_map, &tile_create_ctx->alpha_slider, Vec3i{255, 255, 255});

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

    // Drawing text box
    text_box_parse_key_list(&tile_create_ctx->text, generic_ctx->key_list);
    draw_text_box(pixel_map, &tile_create_ctx->text);
    draw_sentence(pixel_map, "name", 8, 2, Vec2i{80, 708}, Vec3i{255, 255, 0});

    // save btn
    {
	bool hover = is_inside_collision_box(&tile_create_ctx->save_btn.col_box, generic_ctx->mouse_pos);

	draw_button(pixel_map, &tile_create_ctx->save_btn, hover);

	if (generic_ctx->mouse_released && hover)
	{
	    char buf[255] = {0};
	    strcpy(buf, g_tile_dir);
	    strcat(buf, tile_create_ctx->text.text);
	    strcat(buf, g_tile_file_extension);

	    write_tile_to_file(buf, tile_create_ctx->tile, tile_create_ctx->tile_width, tile_create_ctx->tile_height);
	}
    }

    // load btn
    {
	bool hover = is_inside_collision_box(&tile_create_ctx->load_btn.col_box, generic_ctx->mouse_pos);

	draw_button(pixel_map, &tile_create_ctx->load_btn, hover);

	if (generic_ctx->mouse_released && hover)
	{
	    char buf[255] = {0};
	    strcpy(buf, g_tile_dir);
	    strcat(buf, tile_create_ctx->text.text);
	    strcat(buf, g_tile_file_extension);

	    PixelMap new_map = pixel_map_create(tile_create_ctx->tile_width, tile_create_ctx->tile_height);

	    int status = load_tile_from_file(buf, &new_map);

	    if (status == 0)
	    {
		for (int y = 0; y < new_map.height; ++y)
		{
		    for (int x = 0; x < new_map.height; ++x)
		    {
			const int index = y * new_map.width + x;
			tile_create_ctx->tile[index] = new_map.data[index];
		    }
		}
		
		pixel_map_destroy(&new_map);
	    }
	}
    }

    return return_code;
}

void tile_create_end(TileCreateCtx *tile_create_ctx)
{
    free(tile_create_ctx->tile);
    text_box_destroy(&tile_create_ctx->text);
    pixel_map_destroy(&tile_create_ctx->tile_pixmap);
    free(tile_create_ctx);
}
