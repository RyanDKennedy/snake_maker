#include "map.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

SnakeMap* snake_map_create(const char *path)
{
    // Create map
    SnakeMap *map = (SnakeMap*)calloc(1, sizeof(SnakeMap));


    bool error;
    std::string file_contents_str = read_file(path, &error);
    if (error)
    {
	SNAKE_MSG("failed to read file %s\n", path);
	exit(1);
    }
    char *file_contents = (char*)file_contents_str.c_str();
    const int file_len = strlen(file_contents);

    // Split the file into each line
    int line_amt = 1;
    char **lines = NULL;
    {
	// Get amount of lines
	for (int i = 0; i < file_len; ++i)
	{
	    if (file_contents[i] == '\n')
		++line_amt;
	}

	// Allocate and fill lines
	lines = (char**)(calloc(line_amt, sizeof(char*)));
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

    // Read the file and get pointers to each part
    int tile_definitions_index;
    int board_map_index;
    int collision_map_index;
    int end_index;
    {
	for (int i = 0; i < line_amt; ++i)
	{
	    const int line_len = strlen(lines[i]);

	    const char *tile_definitions_str = "[TileDefinitions]";
	    const int tile_definitions_str_len = strlen(tile_definitions_str);
	    if (line_len >= tile_definitions_str_len && memcmp(lines[i], tile_definitions_str, tile_definitions_str_len) == 0)
	    {
		tile_definitions_index = i;
	    }

	    const char *board_map_str = "[BoardMap]";
	    const int board_map_str_len = strlen(board_map_str);
	    if (line_len >= board_map_str_len && memcmp(lines[i], board_map_str, board_map_str_len) == 0)
	    {
		board_map_index = i;
	    }

	    const char *collision_map_str = "[CollisionMap]";
	    const int collision_map_str_len = strlen(collision_map_str);
	    if (line_len >= collision_map_str_len && memcmp(lines[i], collision_map_str, collision_map_str_len) == 0)
	    {
		collision_map_index = i;
	    }

	    const char *end_str = "[END]";
	    const int end_str_len = strlen(end_str);
	    if (line_len >= end_str_len && memcmp(lines[i], end_str, end_str_len) == 0)
	    {
		end_index = i;
	    }
	}
    }

    // Create the tile maps
    map->tiles_amt = board_map_index - tile_definitions_index - 1;
    map->tile_maps = (PixelMap*)calloc(map->tiles_amt, sizeof(PixelMap));
    for (int i = 0; i < map->tiles_amt; ++i)
    {
	const int line_num = tile_definitions_index + i + 1;
	const int line_len = strlen(lines[line_num]);
	
	int tile_number;
	char tile_path[256];
	strcpy(tile_path, g_tile_dir);

	const char *tile_name = lines[line_num];
	for (int ch = 0; ch < line_len; ++ch)
	{
	    if (lines[line_num][ch] == '=')
	    {
		lines[line_num][ch] = '\0';
		tile_number = atoi(&lines[line_num][ch + 1]);
		break;
	    }
	}
	strcat(tile_path, tile_name);
	strcat(tile_path, g_tile_file_extension);

	map->tile_maps[tile_number] = pixel_map_create(10, 10);
	load_tile_from_file(tile_path, &map->tile_maps[tile_number]);
    }

    // Get the board map
    load_grid_map(lines + board_map_index + 1, collision_map_index - board_map_index - 1, map->board_map);

    // Get the collision map
    load_grid_map(lines + collision_map_index + 1, end_index - collision_map_index - 1, map->collision_map);

    // Create board pixel map
    map->board_pixel_map = pixel_map_create(600, 600);
    draw_snake_map(&map->board_pixel_map, map);

    free(lines);
    
    return map;
}

void snake_map_destroy(SnakeMap *map)
{
    for (int i = 0; i < map->tiles_amt; ++i)
    {
	pixel_map_destroy(&map->tile_maps[i]);
    }
    free(map->tile_maps);

    pixel_map_destroy(&map->board_pixel_map);
}

void load_tile_from_file(const char *path, PixelMap *target_map)
{
    bool error;
    std::string file_contents_str = read_file(path, &error);
    if (error)
    {
	SNAKE_MSG("failed to read file %s\n", path);
	exit(1);
    }

    char *file_contents = (char*)file_contents_str.c_str();
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

	for (int x = 0; x < parens_amt; ++x)
	{
	    // fill color channels

	    const int index = (line_amt - line_num - 1) * parens_amt + x; 
	    target_map->data[index].r = atoi(parens[x] + 1);

	    const char *commas[2];
	    const int len = strlen(parens[x]);
	    num = 0;
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

void load_grid_map(char **lines, int length, int *map)
{
    // count commas
    int comma_amt = 0;
    ++comma_amt;
    for (int i = 0; i < strlen(lines[0]); ++i)
    {
	if (lines[0][i] == ',')
	    ++comma_amt;
    }
    
    // Go through each line
    int *comma_indices = (int*)calloc(comma_amt, sizeof(int));
    for (int line_num = 0; line_num < length; ++line_num)
    {
	// Get comma indices on the line
	comma_amt = 0;
	comma_indices[comma_amt++] = -1;
	for (int i = 0; i < strlen(lines[0]); ++i)
	{
	    if (lines[0][i] == ',')
	    {
		comma_indices[comma_amt++] = i;
	    }	    
	}
	
	// Fill in the data
	for (int i = 0; i < comma_amt; ++i)
	{
	    map[line_num*length + i] = atoi(&lines[line_num][comma_indices[i] + 1]);
	}

    }

    free(comma_indices);
}

void draw_snake_map(PixelMap *target_map, SnakeMap *map)
{
    for (int y = 0; y < map->height; ++y)
    {
	for (int x = 0; x < map->width; ++x)
	{
	    const int current_index = map->board_map[y * map->width + x];
	    PixelMap *current_tile_map = &map->tile_maps[current_index];
	    
	    draw_pixmap(target_map, current_tile_map, Vec2i{x * current_tile_map->width, y * current_tile_map->height});
	}
    }
}
