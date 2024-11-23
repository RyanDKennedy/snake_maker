#pragma once

#include "common.hpp"
#include "pixel_map.hpp"
#include "math_types.hpp"
#include "utils.hpp"
#include "drawing.hpp"

struct SnakeMap
{
    PixelMap board_pixel_map;
    static const int width = 60;
    static const int height = 60;
    static const int size = width * height;

    int collision_map[size];
    int board_map[size]; // this is a map of indices to tile_maps array
    PixelMap *tile_maps; // this is an array of each tile's pixel map
    int tiles_amt;
};

SnakeMap* snake_map_create(const char *path);
void snake_map_destroy(SnakeMap *map);

void load_tile_from_file(const char *path, PixelMap *target_map);
void write_tile_to_file(const char *path, RGBPixel *tile_data, int tile_width, int tile_height);
void load_grid_map(char **lines, int length, int *map);

void draw_snake_map(PixelMap *target_map, SnakeMap *map);
