#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"

struct MapCreateCtx
{


};


MapCreateCtx* map_create_start(GenericCtx *generic_ctx);

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx);

void map_create_end(MapCreateCtx *map_create_ctx);
