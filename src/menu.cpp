#include "menu.hpp"
#include "collisions.hpp"
#include "game_state.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>

MenuCtx* menu_start(GenericCtx *generic_context)
{
    MenuCtx *context = (MenuCtx*)calloc(1, sizeof(MenuCtx));

    // Play snake btn
    context->play_snake_btn = create_button("play snake", 300, 50, (Vec2i){250, 600}, (Vec3i){50, 50, 50}, (Vec3i){255, 255, 255}, 8, 2);
    memcpy(context->play_snake_btn.bg_hover_color, (Vec3i){50, 50, 100}, sizeof(Vec3i));
    

    // Create map btn
    context->create_map_btn = create_button("create map", 300, 50, (Vec2i){250, 530}, (Vec3i){50, 50, 50}, (Vec3i){255, 255, 255}, 8, 2);
    memcpy(context->create_map_btn.bg_hover_color, (Vec3i){50, 50, 100}, sizeof(Vec3i));

    // Settings btn
    context->settings_btn = create_button("settings", 300, 50, (Vec2i){250, 460}, (Vec3i){50, 50, 50}, (Vec3i){255, 255, 255}, 8, 2);
    memcpy(context->settings_btn.bg_hover_color, (Vec3i){50, 50, 100}, sizeof(Vec3i));


    return context;
}

GameReturnCode menu_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MenuCtx *menu_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    draw_sentence(pixel_map, "main menu", 8, 3, (Vec2i){290, 689}, (Vec3i){255, 255, 0});

    // Play snake button
    {
	bool hover = is_inside_collision_box(&menu_ctx->play_snake_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::play_snake;
	}
	draw_button(pixel_map, &menu_ctx->play_snake_btn, hover);
    }

    // Play snake button
    {
	bool hover = is_inside_collision_box(&menu_ctx->create_map_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::none;
	}
	draw_button(pixel_map, &menu_ctx->create_map_btn, hover);
    }

    // Play snake button
    {
	bool hover = is_inside_collision_box(&menu_ctx->settings_btn.col_box, generic_ctx->mouse_pos);
	if (hover && generic_ctx->mouse_clicked)
	{
	    return_code = GameReturnCode::none;
	}
	draw_button(pixel_map, &menu_ctx->settings_btn, hover);
    }

    return return_code;
}

void menu_end(MenuCtx *menu_ctx)
{
    

    free(menu_ctx);
}
