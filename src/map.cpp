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
    int settings_index;
    int tile_definitions_index;
    int board_map_index;
    int collision_map_index;
    int end_index;
    {
	for (int i = 0; i < line_amt; ++i)
	{
	    const int line_len = strlen(lines[i]);

	    const char *settings_str = "[Settings]";
	    const int settings_str_len = strlen(settings_str);
	    if (line_len >= settings_str_len && memcmp(lines[i], settings_str, settings_str_len) == 0)
	    {
		settings_index = i;
		continue;
	    }

	    const char *tile_definitions_str = "[TileDefinitions]";
	    const int tile_definitions_str_len = strlen(tile_definitions_str);
	    if (line_len >= tile_definitions_str_len && memcmp(lines[i], tile_definitions_str, tile_definitions_str_len) == 0)
	    {
		tile_definitions_index = i;
		continue;
	    }

	    const char *board_map_str = "[BoardMap]";
	    const int board_map_str_len = strlen(board_map_str);
	    if (line_len >= board_map_str_len && memcmp(lines[i], board_map_str, board_map_str_len) == 0)
	    {
		board_map_index = i;
		continue;
	    }

	    const char *collision_map_str = "[CollisionMap]";
	    const int collision_map_str_len = strlen(collision_map_str);
	    if (line_len >= collision_map_str_len && memcmp(lines[i], collision_map_str, collision_map_str_len) == 0)
	    {
		collision_map_index = i;
		continue;
	    }

	    const char *end_str = "[END]";
	    const int end_str_len = strlen(end_str);
	    if (line_len >= end_str_len && memcmp(lines[i], end_str, end_str_len) == 0)
	    {
		end_index = i;
		continue;
	    }
	}
    }


    // Parse the settings
    {
#define CREATE_KEY_STR_VAR(name)			\
    const char *name##_str = #name "=";			\
    const size_t name##_str_len = strlen(name##_str);

#define KEY_STR_COND(name)						\
    (strlen(lines[i]) > name##_str_len && memcmp(lines[i], name##_str, name##_str_len) == 0)
    
#define KEY_STR_VALUE(name) &lines[i][name##_str_len]

	CREATE_KEY_STR_VAR(starting_x)
	CREATE_KEY_STR_VAR(starting_y);
	CREATE_KEY_STR_VAR(starting_direction);
	CREATE_KEY_STR_VAR(width);
	CREATE_KEY_STR_VAR(height);
	CREATE_KEY_STR_VAR(tile_width);
	CREATE_KEY_STR_VAR(tile_height);

	for (int i = settings_index; i < tile_definitions_index; ++i)
	{
	    if (KEY_STR_COND(starting_x))
	    {
		map->starting_pos[0] = atoi(KEY_STR_VALUE(starting_x));
	    }
	    else if (KEY_STR_COND(starting_y))
	    {
		map->starting_pos[1] = atoi(KEY_STR_VALUE(starting_y));
	    }
	    else if (KEY_STR_COND(width))
	    {
		map->width = atoi(KEY_STR_VALUE(width));

	    }
	    else if (KEY_STR_COND(height))
	    {
		map->height = atoi(KEY_STR_VALUE(height));
	    }
	    else if (KEY_STR_COND(tile_width))
	    {
		map->tile_width = atoi(KEY_STR_VALUE(tile_width));

	    }
	    else if (KEY_STR_COND(tile_height))
	    {
		map->tile_height = atoi(KEY_STR_VALUE(tile_height));
	    }
	    else if (KEY_STR_COND(starting_direction))
	    {
		map->starting_direction = atoi(KEY_STR_VALUE(starting_direction));
	    }

	}

	// Calculated values
	map->size = map->width * map->height;

#undef CREATE_KEY_STR_VAR
#undef KEY_STR_COND
#undef KEY_STR_VALUE

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

	map->tile_maps[tile_number] = pixel_map_create(map->tile_width, map->tile_height);
	int status = load_tile_from_file(tile_path, &map->tile_maps[tile_number]);
	if (status != 0)
	{
	    free(lines);
	    exit(1);
	}
    }

    // Get the board map
    map->board_map = (int*)calloc(map->size, sizeof(int));
    load_grid_map(lines + board_map_index + 1, map->board_map, map->width, map->height);

    // Get the collision map
    map->collision_map = (int*)calloc(map->size, sizeof(int));
    load_grid_map(lines + collision_map_index + 1, map->collision_map, map->width, map->height);

    // Create board pixel map
    map->board_pixel_map = pixel_map_create(map->width * map->tile_width, map->height * map->tile_height);
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

    free(map->board_map);
    free(map->collision_map);

    pixel_map_destroy(&map->board_pixel_map);

    free(map);
}

int load_tile_from_file(const char *path, PixelMap *target_map)
{
    bool error;
    std::string file_contents_str = read_file(path, &error);
    if (error)
    {
	SNAKE_MSG("failed to read file %s\n", path);
	return -1;
    }

    char *file_contents = (char*)file_contents_str.c_str();
    const int file_len = strlen(file_contents);

    // Split into lines
    int line_amt = target_map->height;
    char **lines = (char**)calloc(line_amt, sizeof(char*));
    {
	int num = 0;
	lines[num++] = file_contents;
	for (int i = 0; i < file_len && num < line_amt; ++i)
	{
	    if (file_contents[i] == '\n')
	    {
		file_contents[i] = '\0';
		lines[num++] = file_contents + i + 1;
	    }
	}
    }
    
    // Iterate across lines
    int parens_amt = target_map->width;
    char **parens = (char**)calloc(parens_amt, sizeof(char*));
    
    for (int line_num = 0; line_num < line_amt; ++line_num)
    {
	char *current_line = lines[line_num];
	const int current_line_len = strlen(current_line);
	
	// Split into colors

	int num = 0;
	for (int i = 0; i < current_line_len && num < parens_amt; ++i)
	{
	    if (current_line[i] == ')')
	    {
		current_line[i] = '\0';
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

	    const int index = line_num * parens_amt + x; 
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

    free(parens);

    free(lines);

    return 0;
}

void write_tile_to_file(const char *path, RGBPixel *tile_data, int tile_width, int tile_height)
{
    FILE *fd = fopen(path, "w+");
    if (fd == NULL)
    {
	exit(1);
    }

    char buf[256];
    for (int y = 0; y < tile_height - 1; ++y)
    {
	for (int x = 0; x < tile_width - 1; ++x)
	{
	    const int index = y * tile_width + x;
	    snprintf(buf, 256, "(%d, %d, %d), ", tile_data[index].r, tile_data[index].g, tile_data[index].b);
	    fwrite(buf, sizeof(char), strlen(buf), fd);
	}
	int x = tile_width - 1;
	const int index = y * tile_width + x;
	snprintf(buf, 256, "(%d, %d, %d),\n", tile_data[index].r, tile_data[index].g, tile_data[index].b);
	fwrite(buf, sizeof(char), strlen(buf), fd);
    }

    int y = tile_height - 1;
    for (int x = 0; x < tile_width - 1; ++x)
    {
	const int index = y * tile_width + x;
	snprintf(buf, 256, "(%d, %d, %d), ", tile_data[index].r, tile_data[index].g, tile_data[index].b);
	fwrite(buf, sizeof(char), strlen(buf), fd);
    }
    int x = tile_width - 1;
    const int index = y * tile_width + x;
    snprintf(buf, 256, "(%d, %d, %d),", tile_data[index].r, tile_data[index].g, tile_data[index].b);
    fwrite(buf, sizeof(char), strlen(buf), fd);
    
    fclose(fd);
}

void load_grid_map(char **lines, int *map, int width, int height)
{
    // count commas
    int comma_amt = width;

    // Go through each line
    int *comma_indices = (int*)calloc(comma_amt, sizeof(int));
    for (int line_num = 0; line_num < height; ++line_num)
    {
	// Get comma indices on the line
	int num = 0;
	comma_indices[num++] = -1;
	for (int i = 0; i < strlen(lines[0]) && num < width; ++i)
	{
	    if (lines[0][i] == ',')
	    {
		comma_indices[num++] = i;
	    }	    
	}

	// Fill in the data
	for (int i = 0; i < comma_amt; ++i)
	{
	    map[line_num*width + i] = atoi(&lines[line_num][comma_indices[i] + 1]);
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
