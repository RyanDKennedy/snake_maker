#pragma once

#include "context.hpp"
#include "button.hpp"
#include "drawing.hpp"
#include "pixel_map.hpp"
#include "game_state.hpp"
#include "collisions.hpp"

struct MenuCtx
{
    Button play_snake_btn;
};

MenuCtx* menu_start(GenericCtx *generic_context);
GameReturnCode menu_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MenuCtx *menu_ctx);
