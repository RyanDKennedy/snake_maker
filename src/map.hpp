#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "math_types.hpp"
#include "utils.hpp"
#include "drawing.hpp"
#include "quad.hpp"
#include "shader.hpp"

struct SnakeMap
{
    int width;
    int height;
    int size;

    int tile_width;
    int tile_height;

    int *collision_map;
    int *board_map; // this is a map of indices to tile_maps array

    char **tile_names;
    PixelMap *tile_maps; // array of pixelmaps for each tile at index of number in map file
    int tiles_amt;
    GLuint *tile_textures;
    Quad tile_quad;
    Shader tile_shader;

    Vec2i starting_pos;
    int starting_direction; // 0 = up, 1 = right, 2 = down, 3 = left

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
};

SnakeMap* snake_map_create(const char *path);
void snake_map_destroy(SnakeMap *map);

int load_tile_from_file(const char *path, PixelMap *target_map);
void write_tile_to_file(const char *path, RGBAPixel *tile_data, int tile_width, int tile_height);
void load_grid_map(char **lines, int *map, int width, int height);
void load_skin(const char *path, SnakeMap *map);

void draw_snake_map(SnakeMap *map, int width, int height, Vec2i pos, glm::mat4 vp);
