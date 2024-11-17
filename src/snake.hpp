#pragma once

#include "pixel_map.hpp"
#include "drawing.hpp"
#include "context.hpp"

struct SnakeCtx
{
    char map_path[255];

};

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name);

void snake_run(GenericCtx *generic_ctx, SnakeCtx *snake_ctx);
