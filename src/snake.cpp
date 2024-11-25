#include "snake.hpp"
#include "common.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "map.hpp"

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
    head.pos[0] = snake_ctx->map->starting_pos[0];
    head.pos[1] = snake_ctx->map->starting_pos[1];
    snake_ctx->snake.push_front(head);

    switch (snake_ctx->map->starting_direction)
    {
	case 0:
	    snake_ctx->snake_dir = SnakeDir::up;
	    break;
	case 1:
	    snake_ctx->snake_dir = SnakeDir::right;
	    break;
	case 2:
	    snake_ctx->snake_dir = SnakeDir::down;
	    break;
	case 3:
	    snake_ctx->snake_dir = SnakeDir::left;
	    break;
    }
    snake_ctx->snake_dir_old = snake_ctx->snake_dir;

    snake_ctx->add_segments = generic_ctx->settings.starting_size;

    snake_ctx->apples_amt = 2;
    snake_ctx->apples = (Vec2i*)calloc(snake_ctx->apples_amt, sizeof(Vec2i));
    for (int i = 0; i < snake_ctx->apples_amt; ++i)
    {
	do
	{
	    snake_ctx->apples[i][0] = rand() % snake_ctx->map->width;
	    snake_ctx->apples[i][1] = rand() % snake_ctx->map->width;
	} while (snake_ctx->map->collision_map[snake_ctx->apples[i][0] + snake_ctx->apples[i][1] * snake_ctx->map->width] != 0);
    }

    snake_ctx->speed = 1.0 / generic_ctx->settings.starting_speed;
    snake_ctx->time_since_last_move = 0;

    return snake_ctx;
}

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    const int border_size = 15;

    Vec2i map_pos;
    map_pos[0] = pixel_map->width / 2 - (snake_ctx->map->width * snake_ctx->map->tile_width) / 2;
    map_pos[1] = pixel_map->height / 2 - (snake_ctx->map->height * snake_ctx->map->tile_height) / 2;

    Vec2i border_pos;
    border_pos[0] = map_pos[0] - border_size;
    border_pos[1] = map_pos[1] - border_size;

    // Draw outline
    draw_rectangle(pixel_map, (snake_ctx->map->width * snake_ctx->map->tile_width) + 2*border_size, (snake_ctx->map->height * snake_ctx->map->tile_height) + 2*border_size, border_pos, Vec3i{50, 50, 50});
    
    // Draw map
    draw_pixmap(pixel_map, &snake_ctx->map->board_pixel_map, map_pos);

    // Parse Input
    switch (generic_ctx->last_pressed_key)
    {
	case 'W':
	    if (snake_ctx->snake_dir_old != SnakeDir::down)
		snake_ctx->snake_dir = SnakeDir::up;
	    break;
	case 'A':
	    if (snake_ctx->snake_dir_old != SnakeDir::right)
		snake_ctx->snake_dir = SnakeDir::left;
	    break;
	case 'S':
	    if (snake_ctx->snake_dir_old != SnakeDir::up)
		snake_ctx->snake_dir = SnakeDir::down;
	    break;
	case 'D':
	    if (snake_ctx->snake_dir_old != SnakeDir::left)
		snake_ctx->snake_dir = SnakeDir::right;
	    break;
	default:
	    break;
    }

    float speed = snake_ctx->speed;
    if (generic_ctx->keyboard.space)
    {
	speed /= 2;
    }


    // Update snake
    snake_ctx->time_since_last_move += generic_ctx->delta_time;
    while (snake_ctx->time_since_last_move >= speed)
    {
	snake_ctx->time_since_last_move -= speed;

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
	draw_pixmap(pixel_map, &snake_ctx->map->apple_tile, Vec2i{map_pos[0] + snake_ctx->map->tile_width * snake_ctx->apples[i][0], map_pos[1] + snake_ctx->map->tile_height * snake_ctx->apples[i][1]});
//	draw_rectangle(pixel_map, snake_ctx->map->tile_width, snake_ctx->map->tile_height, Vec2i{map_pos[0] + snake_ctx->map->tile_width * snake_ctx->apples[i][0], map_pos[1] + snake_ctx->map->tile_height * snake_ctx->apples[i][1]}, Vec3i{255, 0, 0});
    }

    // Draw snake
    {
	PixelMap *pix_map;

	switch (snake_ctx->snake_dir_old)
	{
	    case SnakeDir::up:
		pix_map = &snake_ctx->map->snake_skin.head_up;
		break;
	    case SnakeDir::down:
		pix_map = &snake_ctx->map->snake_skin.head_down;
		break;
	    case SnakeDir::left:
		pix_map = &snake_ctx->map->snake_skin.head_left;
		break;
	    case SnakeDir::right:
		pix_map = &snake_ctx->map->snake_skin.head_right;
		break;
	}

	draw_pixmap(pixel_map, pix_map, Vec2i{map_pos[0] + snake_ctx->map->tile_width * snake_ctx->snake.front().pos[0], map_pos[1] + snake_ctx->map->tile_height * snake_ctx->snake.front().pos[1]});

	for (int i = 1; i < snake_ctx->snake.size() - 1; ++i)
	{

	    int index1 = (snake_ctx->snake[i - 1].pos[0] < snake_ctx->snake[i + 1].pos[0])? i - 1 : i + 1;
	    int index2 = (snake_ctx->snake[i - 1].pos[0] < snake_ctx->snake[i + 1].pos[0])? i + 1 : i - 1;
	    
	    Vec2i pos1;
	    pos1[0] = snake_ctx->snake[index1].pos[0];
	    pos1[1] = snake_ctx->snake[index1].pos[1];
	    Vec2i pos2;
	    pos2[0] = snake_ctx->snake[index2].pos[0];
	    pos2[1] = snake_ctx->snake[index2].pos[1];
	    
	    float dx = pos1[0] - pos2[0];
	    float dy = pos1[1] - pos2[1];
	    
	    if (dx == 0 && dy != 0)
	    {
		pix_map = &snake_ctx->map->snake_skin.vertical;
	    }
	    else if (dy == 0 && dx != 0)
	    {
		pix_map = &snake_ctx->map->snake_skin.horizontal;
	    }
	    else if (dx == dy && pos1[1] != snake_ctx->snake[i].pos[1])
	    {
		pix_map = &snake_ctx->map->snake_skin.right_down;
	    }
	    else if (dx == -dy && pos1[1] != snake_ctx->snake[i].pos[1])
	    {
		pix_map = &snake_ctx->map->snake_skin.right_up;
	    }
	    else if (dx == dy && pos1[1] == snake_ctx->snake[i].pos[1])
	    {
		pix_map = &snake_ctx->map->snake_skin.left_up;
	    }
	    else if (dx == -dy && pos1[1] == snake_ctx->snake[i].pos[1])
	    {
		pix_map = &snake_ctx->map->snake_skin.left_down;
	    }

	    draw_pixmap(pixel_map, pix_map, Vec2i{map_pos[0] + snake_ctx->map->tile_width * snake_ctx->snake[i].pos[0], map_pos[1] + snake_ctx->map->tile_height * snake_ctx->snake[i].pos[1]});
	}
	int dx = snake_ctx->snake[snake_ctx->snake.size() - 2].pos[0] - snake_ctx->snake.back().pos[0];
	int dy = snake_ctx->snake[snake_ctx->snake.size() - 2].pos[1] - snake_ctx->snake.back().pos[1];
	if (dx == 1)
	    pix_map = &snake_ctx->map->snake_skin.tail_right;
	else if (dx == -1)
	    pix_map = &snake_ctx->map->snake_skin.tail_left;
	else if (dy == 1)
	    pix_map = &snake_ctx->map->snake_skin.tail_up;
	else if (dy == -1)
	    pix_map = &snake_ctx->map->snake_skin.tail_down;

	draw_pixmap(pixel_map, pix_map, Vec2i{map_pos[0] + snake_ctx->map->tile_width * snake_ctx->snake.back().pos[0], map_pos[1] + snake_ctx->map->tile_height * snake_ctx->snake.back().pos[1]});
    }
    

    if (generic_ctx->keyboard.escape)
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
