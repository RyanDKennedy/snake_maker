#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "math_types.hpp"
#include "utils.hpp"
#include "drawing.hpp"
#include "quad.hpp"

struct SnakeMap
{
    PixelMap board_pixel_map;
    int width;
    int height;
    int size;

    int tile_width;
    int tile_height;

    int *collision_map;
    int *board_map; // this is a map of indices to tile_maps array
    PixelMap *tile_maps; // this is an array of each tile's pixel map
    int tiles_amt;

    Vec2i starting_pos;
    int starting_direction; // 0 = up, 1 = right, 2 = down, 3 = left

    GLuint texture;
    float aspect_ratio;

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

    struct
    {
	PixelMap apple_tile;
	PixelMap vertical;
	PixelMap horizontal;	
	PixelMap left_up;	
	PixelMap right_up;
	PixelMap left_down;	
	PixelMap right_down;

	PixelMap tail_down;
	PixelMap tail_up;
	PixelMap tail_left;
	PixelMap tail_right;

	PixelMap head_down;
	PixelMap head_up;
	PixelMap head_left;
	PixelMap head_right;	
    } skin;
};

SnakeMap* snake_map_create(const char *path, bool texture_filtering);
void snake_map_destroy(SnakeMap *map);

int load_tile_from_file(const char *path, PixelMap *target_map);
void write_tile_to_file(const char *path, RGBAPixel *tile_data, int tile_width, int tile_height);
void load_grid_map(char **lines, int *map, int width, int height);
void load_skin(const char *path, SnakeMap *map);


void draw_snake_map(PixelMap *target_map, SnakeMap *map);
