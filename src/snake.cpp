#include "snake.hpp"
#include "common.hpp"
#include "game_state.hpp"
#include "map.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name)
{
    // use raii here so that the std::queue gets it's constructor called
    SnakeCtx *snake_ctx = new SnakeCtx;
    strcpy(snake_ctx->map_path, g_map_dir);
    strcat(snake_ctx->map_path, map_name);
    strcat(snake_ctx->map_path, g_map_file_extension);
    snake_ctx->map = snake_map_create(snake_ctx->map_path);

    SnakeSegment head;
    head.pos[0] = 30;
    head.pos[1] = 30;
    snake_ctx->snake.push_front(head);
    snake_ctx->snake_dir = SnakeDir::up;

    snake_ctx->time_since_last_move = 0.0f;

    snake_ctx->add_segments = 5;

    return snake_ctx;
}

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    // Draw outline
    draw_rectangle(pixel_map, 650, 650, Vec2i{75, 75}, Vec3i{50, 50, 50});
    
    // Draw map
    draw_pixmap(pixel_map, &snake_ctx->map->board_pixel_map, Vec2i{100, 100});


    // Parse Input
    switch (generic_ctx->last_pressed_key)
    {
	case 'w':
	    snake_ctx->snake_dir = SnakeDir::up;
	    break;
	case 'a':
	    snake_ctx->snake_dir = SnakeDir::left;
	    break;
	case 's':
	    snake_ctx->snake_dir = SnakeDir::down;
	    break;
	case 'd':
	    snake_ctx->snake_dir = SnakeDir::right;
	    break;
	default:
	    break;
    }


    // Update snake
    snake_ctx->time_since_last_move += generic_ctx->delta_time;
    while (snake_ctx->time_since_last_move > 0.2)
    {
	snake_ctx->time_since_last_move -= 0.2;

	int x_translate = 0;
	int y_translate;

	switch(snake_ctx->snake_dir)
	{
	    case SnakeDir::up:
		y_translate = 1;
		x_translate = 0;
		break;
	    case SnakeDir::down:
		y_translate = -1;
		x_translate = 0;
		break;
	    case SnakeDir::left:
		y_translate = 0;
		x_translate = -1;
		break;
	    case SnakeDir::right:
		y_translate = 0;
		x_translate = 1;
		break;
	}

	SnakeSegment seg;
	seg.pos[0] = snake_ctx->snake.front().pos[0] + x_translate;
	seg.pos[1] = snake_ctx->snake.front().pos[1] + y_translate;
	snake_ctx->snake.push_front(seg);

	if (snake_ctx->add_segments == 0)
	{
	    snake_ctx->snake.pop_back();
	}
	else
	{
	    --snake_ctx->add_segments;
	}

    }


    // Draw snake
    for (int i = 0; i < snake_ctx->snake.size(); ++i)
    {
	draw_rectangle(pixel_map, 10, 10, Vec2i{100 + 10 * snake_ctx->snake[i].pos[0], 100 + 10 * snake_ctx->snake[i].pos[1]}, Vec3i{0, 255, 0});
    }


    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

    return return_code;
}

void snake_end(SnakeCtx *snake_ctx)
{
    snake_map_destroy(snake_ctx->map);
    delete snake_ctx;
}
