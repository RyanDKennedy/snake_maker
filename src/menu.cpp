#include "menu.hpp"
#include "collisions.hpp"
#include "game_state.hpp"

#include <cstring>
#include <cstdlib>

MenuCtx* menu_start(GenericCtx *generic_context)
{
    MenuCtx *context = (MenuCtx*)calloc(1, sizeof(MenuCtx));
    context->play_snake_btn = create_button("play snake", 300, 50, (Vec2i){400, 400}, (Vec3i){50, 50, 50}, (Vec3i){255, 255, 255}, 8, 2);
    memcpy(context->play_snake_btn.bg_hover_color, (Vec3i){50, 50, 100}, sizeof(Vec3i));
    
    return context;
}

GameReturnCode menu_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MenuCtx *menu_ctx)
{
    bool hover_play_snake = is_inside_collision_box(&menu_ctx->play_snake_btn.col_box, generic_ctx->mouse_pos);
    if (hover_play_snake && generic_ctx->mouse_clicked)
    {
	return GameReturnCode::play_snake;
    }

    draw_button(pixel_map, &menu_ctx->play_snake_btn, hover_play_snake);


    return GameReturnCode::none;
}
