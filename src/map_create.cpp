#include "map_create.hpp"
#include "collisions.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "text_box.hpp"

#include <cstdlib>

MapCreateCtx* map_create_start(GenericCtx *generic_ctx)
{
    MapCreateCtx *ctx = (MapCreateCtx*)calloc(1, sizeof(MapCreateCtx));

    ctx->view_buttons[BOARD_MAP_VIEW] = create_button("board map", 180, 40, Vec2i{10, 700}, Vec3i{50, 50, 50}, Vec3i{150, 150, 0}, 8, 2);
    memcpy(&ctx->view_buttons[BOARD_MAP_VIEW].bg_hover_color, &ctx->view_buttons[BOARD_MAP_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[BOARD_MAP_VIEW].fg_hover_color, &ctx->view_buttons[BOARD_MAP_VIEW].bg_color, sizeof(Vec3i));

    ctx->view_buttons[TILE_MANAGER_VIEW] = create_button("tile manager", 210, 40, Vec2i{200, 700}, Vec3i{50, 50, 50}, Vec3i{150, 150, 0}, 8, 2);
    memcpy(&ctx->view_buttons[TILE_MANAGER_VIEW].bg_hover_color, &ctx->view_buttons[TILE_MANAGER_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[TILE_MANAGER_VIEW].fg_hover_color, &ctx->view_buttons[TILE_MANAGER_VIEW].bg_color, sizeof(Vec3i));

    ctx->view_buttons[COLLISION_MAP_VIEW] = create_button("collision map", 210, 40, Vec2i{10, 650}, Vec3i{50, 50, 50}, Vec3i{150, 150, 0}, 8, 2);
    memcpy(&ctx->view_buttons[COLLISION_MAP_VIEW].bg_hover_color, &ctx->view_buttons[COLLISION_MAP_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[COLLISION_MAP_VIEW].fg_hover_color, &ctx->view_buttons[COLLISION_MAP_VIEW].bg_color, sizeof(Vec3i));

    ctx->view_buttons[SETTINGS_VIEW] = create_button("settings", 180, 40, Vec2i{230, 650}, Vec3i{50, 50, 50}, Vec3i{150, 150, 0}, 8, 2);
    memcpy(&ctx->view_buttons[SETTINGS_VIEW].bg_hover_color, &ctx->view_buttons[SETTINGS_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[SETTINGS_VIEW].fg_hover_color, &ctx->view_buttons[SETTINGS_VIEW].bg_color, sizeof(Vec3i));

    ctx->current_view = BOARD_MAP_VIEW;

    ctx->selector = text_box_create(21, Vec2i{420, 704}, 8, 2);

    ctx->selector_save_btn = create_button("save", 175, 40, Vec2i{420, 650}, Vec3i{50, 50, 50}, Vec3i{0x19, 0xd3, 0x13}, 8, 2);
    memcpy(&ctx->selector_save_btn.bg_hover_color, &ctx->selector_save_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->selector_save_btn.fg_hover_color, &ctx->selector_save_btn.bg_color, sizeof(Vec3i));

    ctx->selector_load_btn = create_button("load", 175, 40, Vec2i{605, 650}, Vec3i{50, 50, 50}, Vec3i{0xff, 0x4f, 0x13}, 8, 2);
    memcpy(&ctx->selector_load_btn.bg_hover_color, &ctx->selector_load_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->selector_load_btn.fg_hover_color, &ctx->selector_load_btn.bg_color, sizeof(Vec3i));



    return ctx;
}

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;
    
    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }

    // Select view buttons
    if (generic_ctx->mouse_released)
    {
	for (int i = 0; i < 4; ++i)
	{
	    bool hover = is_inside_collision_box(&map_create_ctx->view_buttons[i].col_box, generic_ctx->mouse_pos);
	    if (hover)
	    {
		map_create_ctx->current_view = i;
	    }
	}
    }

    // Type in selector
    text_box_parse_key_list(&map_create_ctx->selector, generic_ctx->key_list);

    // Load and save
    {
	bool hover = is_inside_collision_box(&map_create_ctx->selector_load_btn.col_box, generic_ctx->mouse_pos);
	draw_button(pixel_map, &map_create_ctx->selector_load_btn, hover);
    }
    {
	bool hover = is_inside_collision_box(&map_create_ctx->selector_save_btn.col_box, generic_ctx->mouse_pos);
	draw_button(pixel_map, &map_create_ctx->selector_save_btn, hover);
    }

    // Draw view buttons
    for (int i = 0; i < 4; ++i)
    {
	bool selected = (map_create_ctx->current_view == i);
	draw_button(pixel_map, &map_create_ctx->view_buttons[i], selected);
    }

    // Draw selector
    draw_text_box(pixel_map, &map_create_ctx->selector);


    


    switch (map_create_ctx->current_view)
    {
	case BOARD_MAP_VIEW:
	{

	    break;
	}

	case COLLISION_MAP_VIEW:
	{

	    break;
	}

	case TILE_MANAGER_VIEW:
	{


	    break;
	}

	case SETTINGS_VIEW:
	{

	    break;
	}

    }

    return return_code;
}

void map_create_end(MapCreateCtx *map_create_ctx)
{
    text_box_destroy(&map_create_ctx->selector);


    free(map_create_ctx);
}
