#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "math_types.hpp"
#include "utils.hpp"
#include "drawing.hpp"

struct SnakeMap
{
    PixelMap board_pixel_map;
    int width;
    int height;
    int size;

    int tile_width;
    int tile_height;
//    static const int tile_width = 10;
//    static const int tile_height = 10;

    int *collision_map;
    int *board_map; // this is a map of indices to tile_maps array
    PixelMap *tile_maps; // this is an array of each tile's pixel map
    int tiles_amt;

    Vec2i starting_pos;
    int starting_direction; // 0 = up, 1 = right, 2 = down, 3 = left
};

SnakeMap* snake_map_create(const char *path);
void snake_map_destroy(SnakeMap *map);

int load_tile_from_file(const char *path, PixelMap *target_map);
void write_tile_to_file(const char *path, RGBPixel *tile_data, int tile_width, int tile_height);
void load_grid_map(char **lines, int *map, int width, int height);

void draw_snake_map(PixelMap *target_map, SnakeMap *map);
