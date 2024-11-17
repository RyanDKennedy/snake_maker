#include "snake.hpp"
#include "game_state.hpp"
#include "pixel.hpp"
#include "pixel_map.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name)
{
    SnakeCtx *snake_ctx = (SnakeCtx*)calloc(1, sizeof(SnakeCtx));
    strcpy(snake_ctx->map_path, g_map_dir);
    strcat(snake_ctx->map_path, map_name);
    strcat(snake_ctx->map_path, ".snake");
    snake_ctx->snake_pixel_map = pixel_map_create(600, 600);
    memset(snake_ctx->board_map, 0, snake_ctx->map_size);

    { // FIXME:
	snake_ctx->tiles_amt = 1;
	snake_ctx->tile_maps = (PixelMap*)calloc(1, sizeof(PixelMap));
	snake_ctx->tile_infos = (TileInfo*)calloc(1, sizeof(TileInfo));
	snake_ctx->tile_maps[0] = pixel_map_create(10, 10);
	memset(snake_ctx->tile_maps[0].data, 0, snake_ctx->tile_maps[0].size * sizeof(RGBPixel));

	draw_letter(&snake_ctx->tile_maps[0], 'a', 8, 1, Vec2i{1, 1}, Vec3i{255, 255, 255});
	snake_ctx->tile_infos[0].collision = true;
    }

    printf("%s\n", snake_ctx->map_path);

    return snake_ctx;
}

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{

    draw_rectangle(pixel_map, 650, 650, Vec2i{75, 75}, Vec3i{50, 50, 50});
    
    draw_rectangle(&snake_ctx->snake_pixel_map, 600, 600, Vec2i{0, 0}, Vec3i{255, 255, 255});
    snake_draw_base_map(&snake_ctx->snake_pixel_map, snake_ctx);

    draw_pixmap(pixel_map, &snake_ctx->snake_pixel_map, Vec2i{100, 100});

    return GameReturnCode::none;
}

void snake_draw_base_map(PixelMap *target_map, SnakeCtx *snake_ctx)
{
    for (int y = 0; y < snake_ctx->map_height; ++y)
    {
	for (int x = 0; x < snake_ctx->map_width; ++x)
	{
	    const int current_index = snake_ctx->board_map[y * snake_ctx->map_height + x];
	    PixelMap *current_tile_map = &snake_ctx->tile_maps[current_index];
	    
	    draw_pixmap(target_map, current_tile_map, Vec2i{x * current_tile_map->width, y * current_tile_map->height});
	}
    }

}
