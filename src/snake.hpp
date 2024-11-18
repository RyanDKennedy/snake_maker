#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"
#include "context.hpp"
#include "utils.hpp"
#include "game_state.hpp"
#include "map.hpp"

struct SnakeCtx
{
    char map_path[256];
    SnakeMap *map;
};

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name);

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx);

void snake_end(SnakeCtx *snake_ctx);

