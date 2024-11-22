#pragma once

#include "common.hpp"
#include "context.hpp"
#include "button.hpp"
#include "drawing.hpp"
#include "pixel_map.hpp"
#include "game_state.hpp"
#include "collisions.hpp"


struct MenuCtx
{
    Button play_snake_btn;
    Button create_map_btn;
    Button settings_btn;
    Button create_tile_btn;

    Button *available_maps;
    int available_maps_amt;
    int selected_map;
    PixelMap map_selector_pixel_map;
    double map_selector_scroll_amt;
    double min_scroll;
};

MenuCtx* menu_start(GenericCtx *generic_context);
GameReturnCode menu_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MenuCtx *menu_ctx);
void menu_end(MenuCtx *menu_ctx);
void menu_fill_maps(MenuCtx *menu_ctx);
void menu_free_available_maps(MenuCtx *menu_ctx);
