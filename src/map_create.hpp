#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "button.hpp"
#include "text_box.hpp"

#define BOARD_MAP_VIEW 0
#define COLLISION_MAP_VIEW 1
#define TILE_MANAGER_VIEW 2
#define SETTINGS_VIEW 3

struct MapCreateCtx
{
    int current_view;
    
    Button view_buttons[4];

    TextBox selector;
    Button selector_load_btn;
    Button selector_save_btn;


};


MapCreateCtx* map_create_start(GenericCtx *generic_ctx);

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx);

void map_create_end(MapCreateCtx *map_create_ctx);
