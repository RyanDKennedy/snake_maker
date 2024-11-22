#include "tile_create.hpp"
#include "game_state.hpp"

#include <cstdlib>

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx)
{
    TileCreateCtx *ctx = (TileCreateCtx*)calloc(1, sizeof(TileCreateCtx));

    return ctx;
}

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }
    
    return return_code;
}

void tile_create_end(GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{

    free(tile_create_ctx);
}
