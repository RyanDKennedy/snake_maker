#include "snake.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

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
	snake_ctx->tile_maps[0] = pixel_map_create(10, 10);
	memset(snake_ctx->tile_maps[0].data, 0, snake_ctx->tile_maps[0].size * sizeof(RGBPixel));

	draw_letter(&snake_ctx->tile_maps[0], 'a', 8, 1, Vec2i{1, 1}, Vec3i{255, 255, 255});
    }

    load_tile_map_from_block_file("../tiles/solid.tile", &snake_ctx->tile_maps[0]);

    return snake_ctx;
}

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{

    // Draw outline
    draw_rectangle(pixel_map, 650, 650, Vec2i{75, 75}, Vec3i{50, 50, 50});
    
    // Draw board map
    snake_draw_board_map(&snake_ctx->snake_pixel_map, snake_ctx);

    draw_pixmap(pixel_map, &snake_ctx->snake_pixel_map, Vec2i{100, 100});

    return GameReturnCode::none;
}

void snake_draw_board_map(PixelMap *target_map, SnakeCtx *snake_ctx)
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

void load_tile_map_from_block_file(const char *path, PixelMap *target_map)
{
    bool error;
    std::string file_contents_str = read_file(path, &error);
    char *file_contents = (char*)file_contents_str.data();
    const int file_len = strlen(file_contents);

    // Split into lines
    const int line_amt = 10;
    char *lines[line_amt];
    {
	int num = 0;
	lines[num++] = file_contents;
	for (int i = 0; i < file_len; ++i)
	{
	    if (file_contents[i] == '\n')
	    {
		file_contents[i] = '\0';
		lines[num++] = file_contents + i + 1;
	    }
	}
    }
    
    // Iterate across lines
    for (int line_num = 0; line_num < line_amt; ++line_num)
    {
	char *current_line = lines[line_num];
	const int current_line_len = strlen(current_line);
	
	// Split into colors
	const int parens_amt = 10;
	const char *parens[parens_amt];
	int num = 0;
	for (int i = 0; i < current_line_len; ++i)
	{
	    if (current_line[i] == ')')
	    {
		current_line[i + 1] = '\0';
	    }
	    if (current_line[i] == '(')
	    {
		parens[num++] = current_line + i;
	    }	  
	}

	// Iterate across colors
	num = 0;
	for (int x = 0; x < parens_amt; ++x)
	{
	    // fill color channels

	    const int index = (line_amt - line_num - 1) * parens_amt + x; 
	    target_map->data[index].r = atoi(parens[x] + 1);

	    const char *commas[2];
	    const int len = strlen(parens[x]);
	    for (int i = 0; i < len && num < 2; ++i)
	    {
		if (*(parens[x] + i) == ',')
		    commas[num++] = parens[x] + i;
	    }

	    target_map->data[index].g = atoi(commas[0] + 1);
	    target_map->data[index].b = atoi(commas[1] + 1);
	}
    }
}
