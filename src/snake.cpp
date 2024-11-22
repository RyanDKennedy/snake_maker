#include "snake.hpp"
#include "common.hpp"
#include "drawing.hpp"
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
    snake_ctx->snake_dir_old = SnakeDir::up;

    snake_ctx->time_since_last_move = 0.0f;

    snake_ctx->add_segments = generic_ctx->settings.starting_size;

    snake_ctx->apples_amt = 2;
    snake_ctx->apples = (Vec2i*)calloc(snake_ctx->apples_amt, sizeof(Vec2i));
    for (int i = 0; i < snake_ctx->apples_amt; ++i)
    {
	snake_ctx->apples[i][0] = -1;
	snake_ctx->apples[i][1] = -1;
    }

    snake_ctx->speed = 1.0 / generic_ctx->settings.starting_speed;

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
	    if (snake_ctx->snake_dir_old != SnakeDir::down)
		snake_ctx->snake_dir = SnakeDir::up;
	    break;
	case 'a':
	    if (snake_ctx->snake_dir_old != SnakeDir::right)
		snake_ctx->snake_dir = SnakeDir::left;
	    break;
	case 's':
	    if (snake_ctx->snake_dir_old != SnakeDir::up)
		snake_ctx->snake_dir = SnakeDir::down;
	    break;
	case 'd':
	    if (snake_ctx->snake_dir_old != SnakeDir::left)
		snake_ctx->snake_dir = SnakeDir::right;
	    break;
	default:
	    break;
    }

    // Update snake
    snake_ctx->time_since_last_move += generic_ctx->delta_time;
    while (snake_ctx->time_since_last_move > snake_ctx->speed)
    {
	snake_ctx->time_since_last_move -= snake_ctx->speed;

	snake_ctx->snake_dir_old = snake_ctx->snake_dir;

	int x_translate = 0;
	int y_translate = 0;

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

	// Move head
	{
	    SnakeSegment seg;
	    seg.pos[0] = snake_ctx->snake.front().pos[0] + x_translate;
	    seg.pos[1] = snake_ctx->snake.front().pos[1] + y_translate;
	    snake_ctx->snake.push_front(seg);
	}

	// Check for head collision
	{
	    // check for collision with collision_map
	    SnakeSegment front = snake_ctx->snake.front();

	    if (snake_ctx->map->collision_map[front.pos[0] + front.pos[1] * snake_ctx->map->width] == 1)
	    {
		return_code = GameReturnCode::goto_menu;
	    }

	    // check for collision with itself
	    SnakeSegment head = snake_ctx->snake.front();
	    for (int i = 1; i < snake_ctx->snake.size(); ++i)
	    {
		SnakeSegment seg = snake_ctx->snake[i];
		if (head.pos[0] == seg.pos[0] && head.pos[1] == seg.pos[1])
		{
		    return_code = GameReturnCode::goto_menu;
		    break;
		}
	    }

	}

	// Check for apple collision
	for (int i = 0; i < snake_ctx->apples_amt; ++i)
	{
	    if (snake_ctx->apples[i][0] == snake_ctx->snake.front().pos[0] &&
		snake_ctx->apples[i][1] == snake_ctx->snake.front().pos[1])
	    {
		snake_ctx->add_segments += 1;
		snake_ctx->apples[i][0] = -1;
		snake_ctx->apples[i][1] = -1;
	    }
	}

	// Spawn apple if needed
	for (int i = 0; i < snake_ctx->apples_amt; ++i)
	{
	    if (snake_ctx->apples[i][0] == -1 && snake_ctx->apples[i][1] == -1)
	    {
		do
		{
		    snake_ctx->apples[i][0] = rand() % snake_ctx->map->width;
		    snake_ctx->apples[i][1] = rand() % snake_ctx->map->width;
		} while (snake_ctx->map->collision_map[snake_ctx->apples[i][0] + snake_ctx->apples[i][1] * snake_ctx->map->width] != 0);
	    }
	}

	// Add segments to snake
	if (snake_ctx->add_segments == 0)
	{
	    snake_ctx->snake.pop_back();
	}
	else
	{
	    --snake_ctx->add_segments;
	}

    }

    // Draw Apples
    for (int i = 0; i < snake_ctx->apples_amt; ++i)
    {
	draw_rectangle(pixel_map, 10, 10, Vec2i{100 + 10 * snake_ctx->apples[i][0], 100 + 10 * snake_ctx->apples[i][1]}, Vec3i{255, 0, 0});
    }


    // Draw snake
    draw_rectangle(pixel_map, 10, 10, Vec2i{100 + 10 * snake_ctx->snake.front().pos[0], 100 + 10 * snake_ctx->snake.front().pos[1]}, Vec3i{150, 90, 0});
    for (int i = 1; i < snake_ctx->snake.size(); ++i)
    {
	draw_rectangle(pixel_map, 10, 10, Vec2i{100 + 10 * snake_ctx->snake[i].pos[0], 100 + 10 * snake_ctx->snake[i].pos[1]}, Vec3i{0, 170, 0});
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
    free(snake_ctx->apples);
    delete snake_ctx;
}
