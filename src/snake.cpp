#include "snake.hpp"
#include "common.hpp"
#include "game_state.hpp"
#include "map.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name)
{
    SnakeCtx *snake_ctx = (SnakeCtx*)calloc(1, sizeof(SnakeCtx));
    strcpy(snake_ctx->map_path, g_map_dir);
    strcat(snake_ctx->map_path, map_name);
    strcat(snake_ctx->map_path, g_map_file_extension);
    snake_ctx->map = snake_map_create(snake_ctx->map_path);

    return snake_ctx;
}

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    // Draw outline
    draw_rectangle(pixel_map, 650, 650, Vec2i{75, 75}, Vec3i{50, 50, 50});
    
    // Draw map
    draw_pixmap(pixel_map, &snake_ctx->map->board_pixel_map, Vec2i{100, 100});

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

    return return_code;
}

void snake_end(SnakeCtx *snake_ctx)
{
    snake_map_destroy(snake_ctx->map);
    free(snake_ctx);
}
