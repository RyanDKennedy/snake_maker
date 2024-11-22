#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"

struct TileCreateCtx
{

};

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx);

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx);

void tile_create_end(GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx);
