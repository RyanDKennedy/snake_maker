#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"
#include "context.hpp"
#include "utils.hpp"
#include "game_state.hpp"

struct SnakeCtx
{
    char map_path[256];
    PixelMap snake_pixel_map;
    static const int map_width = 60;
    static const int map_height = 60;
    static const int map_size = map_width * map_height;

    int board_map[map_width * map_height]; // this is a map of indices to tile_maps array

    PixelMap *tile_maps; // this is an array of each tile's pixel map
    int tiles_amt;
};

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name);

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx);

void snake_draw_board_map(PixelMap *target_map, SnakeCtx *snake_ctx);

void load_tile_map_from_block_file(const char *path, PixelMap *target_map);
