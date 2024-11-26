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

    float map_aspect_ratio;

    Quad map_quad;
    Shader quad_shader;
    GLuint map_texture;

    Quad tile_quad;
    int tile_width;
    int tile_height;

    struct
    {
	GLuint apple_tile;
	GLuint vertical;
	GLuint horizontal;	
	GLuint left_up;	
	GLuint right_up;
	GLuint left_down;	
	GLuint right_down;

	GLuint tail_down;
	GLuint tail_up;
	GLuint tail_left;
	GLuint tail_right;

	GLuint head_down;
	GLuint head_up;
	GLuint head_left;
	GLuint head_right;	
    } skin_textures;

    SnakeCtx();

};

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name);

GameReturnCode snake_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SnakeCtx *snake_ctx);

void snake_end(SnakeCtx *snake_ctx);


