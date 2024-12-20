#pragma once

#include <queue>

#include "common.hpp"
#include "pixel_map.hpp"
#include "drawing.hpp"
#include "context.hpp"
#include "utils.hpp"
#include "game_state.hpp"
#include "map.hpp"
#include "quad.hpp"
#include "shader.hpp"

enum class SnakeDir
{
    up,
    down,
    left,
    right
};

struct SnakeSegment
{
    Vec2i pos;
};

struct SnakeCtx
{
    char map_path[256];
    SnakeMap *map;

    double speed;

    std::deque<SnakeSegment> snake;
    SnakeDir snake_dir;
    SnakeDir snake_dir_old;
    double time_since_last_move;
    int add_segments;
    
    int apples_amt;
    Vec2i *apples;

    float map_width;
    float map_height;
    Vec2i map_pos;

    Shader quad_shader;
    Quad tile_quad;
    int tile_width;
    int tile_height;

    SnakeCtx();

};

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name);

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx);

void snake_end(SnakeCtx *snake_ctx);


