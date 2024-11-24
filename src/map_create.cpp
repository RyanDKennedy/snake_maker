#include "map_create.hpp"
#include "game_state.hpp"

#include <cstdlib>

MapCreateCtx* map_create_start(GenericCtx *generic_ctx)
{
    MapCreateCtx *ctx = (MapCreateCtx*)calloc(1, sizeof(MapCreateCtx));


    return ctx;
}

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;
    
    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }

    return return_code;
}

void map_create_end(MapCreateCtx *map_create_ctx)
{

    free(map_create_ctx);
}
