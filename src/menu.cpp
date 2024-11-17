#include "menu.hpp"
#include "button.hpp"
#include "collisions.hpp"
#include "game_state.hpp"
#include "math_types.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

MenuCtx* menu_start(GenericCtx *generic_context)
{
    MenuCtx *context = (MenuCtx*)calloc(1, sizeof(MenuCtx));
    context->map_selector_pixel_map = pixel_map_create(400, 350);
    context->available_maps = NULL;
    context->available_maps_amt = 0;
    menu_fill_maps(context);
    context->map_selector_scroll_amt = 400 - (context->available_maps_amt + 1) * 60;
    context->min_scroll = context->map_selector_scroll_amt;

    // Play snake btn
    context->play_snake_btn = create_button("play snake", 300, 50, Vec2i{250, 600}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
    memcpy(context->play_snake_btn.bg_hover_color, Vec3i{50, 50, 100}, sizeof(Vec3i));

    // Create map btn
    context->create_map_btn = create_button("create map", 300, 50, Vec2i{250, 530}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
    memcpy(context->create_map_btn.bg_hover_color, Vec3i{50, 50, 100}, sizeof(Vec3i));

    // Settings btn
    context->settings_btn = create_button("settings", 300, 50, Vec2i{250, 460}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
    memcpy(context->settings_btn.bg_hover_color, Vec3i{50, 50, 100}, sizeof(Vec3i));

    return context;
}

GameReturnCode menu_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MenuCtx *menu_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    draw_sentence(pixel_map, "main menu", 8, 3, Vec2i{290, 689}, Vec3i{255, 255, 0});

    // Play snake btn
    {
	bool hover = is_inside_collision_box(&menu_ctx->play_snake_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::play_snake;
	}
	draw_button(pixel_map, &menu_ctx->play_snake_btn, hover);
    }

    // Create map btn
    {
	bool hover = is_inside_collision_box(&menu_ctx->create_map_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::none;
	}
	draw_button(pixel_map, &menu_ctx->create_map_btn, hover);
    }

    // Settings btn
    {
	bool hover = is_inside_collision_box(&menu_ctx->settings_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::none;
	}
	draw_button(pixel_map, &menu_ctx->settings_btn, hover);
    }

    // Scrolling map selector from input
    {
	// keyboard controls
	const int kb_scroll_amt = 300;
	if (generic_ctx->keyboard.w)
	{
	    menu_ctx->map_selector_scroll_amt -= (kb_scroll_amt * generic_ctx->delta_time);
	}
	if (generic_ctx->keyboard.s)
	{
	    menu_ctx->map_selector_scroll_amt += (kb_scroll_amt * generic_ctx->delta_time);
	}

	// scroll wheel controls
	menu_ctx->map_selector_scroll_amt -= 30 * generic_ctx->mouse_scroll;

	// lower bounds
	if(menu_ctx->map_selector_scroll_amt > 20.0)
	{
	    menu_ctx->map_selector_scroll_amt = 20.0; 
	}

	// top bounds
	if (menu_ctx->map_selector_scroll_amt < menu_ctx->min_scroll)
	{
	    menu_ctx->map_selector_scroll_amt = menu_ctx->min_scroll;
	}

    }

    // Draw the map selector
    {
	// title
	draw_sentence(pixel_map, "map selector", 8, 2, Vec2i{290, 380}, Vec3i{255, 255, 255});

	// Surrounding rectangle
	draw_rectangle(&menu_ctx->map_selector_pixel_map, 400, 350, Vec2i{0, 0}, Vec3i{20, 20, 20});

	const int pixmap_x = 200;
	const int pixmap_y = 20;

	// Draw the map options
	for (int i = 0; i < menu_ctx->available_maps_amt; ++i)
	{
	    // handy shorthand pointer to the btn so I don't have to type out that long thing everytime
	    Button *current_btn = &menu_ctx->available_maps[i];

	    // Set button position based on scroll and index
	    move_button(current_btn, Vec2i{50, (menu_ctx->available_maps_amt - i - 1) * 60 + (int)menu_ctx->map_selector_scroll_amt}, false);

	    // Option selecting
	    Button global_space_btn = *current_btn;
	    move_button(&global_space_btn, Vec2i{pixmap_x, pixmap_y}, true);
	    if (is_inside_collision_box(&global_space_btn.col_box, generic_ctx->mouse_pos) && generic_ctx->mouse_clicked)
	    {
		menu_ctx->selected_map = i;
	    }

	    draw_button(&menu_ctx->map_selector_pixel_map, current_btn, (i == menu_ctx->selected_map));
	}

	draw_pixmap(pixel_map, &menu_ctx->map_selector_pixel_map, Vec2i{pixmap_x, pixmap_y});

    }
    

    return return_code;
}

void menu_end(MenuCtx *menu_ctx)
{
    
    menu_free_available_maps(menu_ctx);
    pixel_map_destroy(&menu_ctx->map_selector_pixel_map);

    free(menu_ctx);
}

void menu_fill_maps(MenuCtx *menu_ctx)
{
    menu_free_available_maps(menu_ctx);

    // Get amount of map files
    std::string path = "../maps/";
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
	if (entry.is_regular_file())
	{
	    
		std::string path_name_str = entry.path().string();
		const char* path_name = path_name_str.c_str();
	    if (strlen(path_name) >= 6 && memcmp(path_name + (strlen(path_name) - 6), ".snake", strlen(".snake")) == 0)
	    {
		++menu_ctx->available_maps_amt;
	    }
	}
    }

    // Create the maps
    int num = 0;
    menu_ctx->available_maps = (Button*)calloc(menu_ctx->available_maps_amt, sizeof(Button));
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
	if (entry.is_regular_file())
	{
		std::string path_name_str = entry.path().string();
	    const char *path_name = path_name_str.c_str();
	    if (strlen(path_name) >= 6 && memcmp(path_name + (strlen(path_name) - 6), ".snake", strlen(".snake")) == 0)
	    {
		
		const char *path = strrchr(path_name, '/') + 1;
		const int path_size = strlen(path) - 6;

		char *new_path = (char*)calloc(path_size + 1, sizeof(char));
		memcpy(new_path, path, path_size);
		new_path[path_size] = '\0';

		menu_ctx->available_maps[num] = create_button((const char*)new_path, 300, 50, Vec2i{0, 0}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
		memcpy(menu_ctx->available_maps[num].bg_hover_color, Vec3i{200, 200, 0}, sizeof(Vec3i));
		++num;
	    }
	}
    }

    // Select the top map
    menu_ctx->selected_map = 0;
}

void menu_free_available_maps(MenuCtx *menu_ctx)
{
    if (menu_ctx->available_maps == NULL)
	return;

    for (int i = 0; i < menu_ctx->available_maps_amt; ++i)
    {
	free((char*)menu_ctx->available_maps[i].text);
    }
    free(menu_ctx->available_maps);
    menu_ctx->available_maps = NULL;
    menu_ctx->available_maps_amt = 0;
}
