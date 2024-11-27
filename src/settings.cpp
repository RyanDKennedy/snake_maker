#include "settings.hpp"
#include "collisions.hpp"
#include "game_state.hpp"

#include <cstdlib>
#include <string>
#include <cstring>
#include "utils.hpp"

#define STARTING_SPEED_INDEX 0
#define STARTING_SIZE_INDEX 1
#define TILE_CREATE_WIDTH_INDEX 2
#define TILE_CREATE_HEIGHT_INDEX 3
//#define TEXTURE_FILTERING_INDEX 4
 
SettingsCtx* settings_start(GenericCtx *generic_ctx)
{
    SettingsCtx *ctx = (SettingsCtx*)calloc(1, sizeof(SettingsCtx));
    ctx->in_prog_settings = generic_ctx->settings;

    ctx->values_amt = 4;
    ctx->values = (IncrementValue*)calloc(ctx->values_amt, sizeof(IncrementValue));

    IncrementValue *starting_speed = &ctx->values[STARTING_SPEED_INDEX];
    IncrementValue *starting_size = &ctx->values[STARTING_SIZE_INDEX];
    IncrementValue *tile_create_width = &ctx->values[TILE_CREATE_WIDTH_INDEX];
    IncrementValue *tile_create_height = &ctx->values[TILE_CREATE_HEIGHT_INDEX];
//    IncrementValue *texture_filtering = &ctx->values[TEXTURE_FILTERING_INDEX];

    *starting_speed = create_increment_value(generic_ctx->settings.starting_speed, "starting_speed", Vec2i{200, 500}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
    starting_speed->has_bottom_limit = true;
    starting_speed->bottom_limit = 1;

    *starting_size = create_increment_value(generic_ctx->settings.starting_size, "starting_size", Vec2i{200, 450}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
    starting_size->has_bottom_limit = true;
    starting_size->bottom_limit = 1;

    *tile_create_width = create_increment_value(generic_ctx->settings.tile_create_width, "tile_create_width", Vec2i{200, 400}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
    tile_create_width->has_bottom_limit = true;
    tile_create_width->bottom_limit = 16;

    *tile_create_height = create_increment_value(generic_ctx->settings.tile_create_height, "tile_create_height", Vec2i{200, 350}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
    tile_create_height->has_bottom_limit = true;
    tile_create_height->bottom_limit = 16;

/*
    *texture_filtering = create_increment_value(generic_ctx->settings.texture_filtering, "texture_filtering", Vec2i{200, 300}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
    texture_filtering->has_bottom_limit = true;
    texture_filtering->bottom_limit = 0;
    texture_filtering->has_top_limit = true;
    texture_filtering->top_limit = 1;
*/

    return ctx;
}

GameReturnCode settings_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SettingsCtx *settings_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }

    // Update values
    for (int i = 0; i < settings_ctx->values_amt; ++i)
    {
	update_increment_value(&settings_ctx->values[i], generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);
    }

    // Draw
    draw_sentence(pixel_map, "settings", 8, 2, Vec2i{290, 580}, Vec3i{255, 255, 255});
    for (int i = 0; i < settings_ctx->values_amt; ++i)
    {
	draw_increment_value(pixel_map, &settings_ctx->values[i], generic_ctx->mouse_pos);
    }

    // Load values into temporary settings
    settings_ctx->in_prog_settings.starting_speed = settings_ctx->values[STARTING_SPEED_INDEX].value;
    settings_ctx->in_prog_settings.starting_size = settings_ctx->values[STARTING_SIZE_INDEX].value;
    settings_ctx->in_prog_settings.tile_create_width = settings_ctx->values[TILE_CREATE_WIDTH_INDEX].value;
    settings_ctx->in_prog_settings.tile_create_height = settings_ctx->values[TILE_CREATE_HEIGHT_INDEX].value;
//    settings_ctx->in_prog_settings.texture_filtering = settings_ctx->values[TEXTURE_FILTERING_INDEX].value;

    return return_code;
}

void settings_end(GenericCtx *generic_ctx, SettingsCtx *settings_ctx)
{
    generic_ctx->settings = settings_ctx->in_prog_settings;
    settings_write_to_file(g_settings_file, &generic_ctx->settings);

    free(settings_ctx->values);
    free(settings_ctx);
}

Settings settings_from_file(const char *path)
{
    Settings settings;
    settings.starting_size = 0;
    settings.starting_speed = 0;


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
    
#define CREATE_KEY_STR_VAR(name)			\
    const char *name##_str = #name "=";			\
    const size_t name##_str_len = strlen(name##_str);

#define KEY_STR_COND(name)						\
    (strlen(lines[i]) > name##_str_len && memcmp(lines[i], name##_str, name##_str_len) == 0)
    
#define KEY_STR_VALUE(name) &lines[i][name##_str_len]
    
    CREATE_KEY_STR_VAR(starting_speed);
    CREATE_KEY_STR_VAR(starting_size);
    CREATE_KEY_STR_VAR(tile_create_width);
    CREATE_KEY_STR_VAR(tile_create_height);
//    CREATE_KEY_STR_VAR(texture_filtering);

    for (int i = 0; i < line_amt; ++i)
    {
	if (KEY_STR_COND(starting_speed))
	{
	    settings.starting_speed = atoi(KEY_STR_VALUE(starting_speed));
	}
	else if (KEY_STR_COND(starting_size))
	{
	    settings.starting_size = atoi(KEY_STR_VALUE(starting_size));
	}
	else if (KEY_STR_COND(tile_create_width))
	{
	    settings.tile_create_width = atoi(KEY_STR_VALUE(tile_create_width));
	}
	else if (KEY_STR_COND(tile_create_height))
	{
	    settings.tile_create_height = atoi(KEY_STR_VALUE(tile_create_height));
	}
/*
	else if (KEY_STR_COND(texture_filtering))
	{
	    settings.texture_filtering = atoi(KEY_STR_VALUE(texture_filtering));
	}
*/


    }
#undef CREATE_KEY_STR_VAR
#undef KEY_STR_COND
#undef KEY_STR_VALUE


    free(lines);

    return settings;
}

void settings_write_to_file(const char *path, Settings *settings)
{
    FILE *fd = fopen(path, "w+");
    if (fd == NULL)
    {
	return;
    }

    char buf[256];
    memset(buf, 0, 256);

#define WRITE_FIELD_TO_FILE(name, descriptor)		\
    {\
        const char *name##_str = #name "=";				\
	const size_t name##_str_len = strlen(name##_str);		\
	fwrite(name##_str, sizeof(char), name##_str_len, fd);		\
	size_t amt = snprintf(buf, 256, descriptor "\n", settings->name); \
	fwrite(buf, sizeof(char), amt, fd);				\
    }

    WRITE_FIELD_TO_FILE(starting_speed, "%d");
    WRITE_FIELD_TO_FILE(starting_size, "%d");
    WRITE_FIELD_TO_FILE(tile_create_width, "%d");
    WRITE_FIELD_TO_FILE(tile_create_height, "%d");
//    WRITE_FIELD_TO_FILE(texture_filtering, "%d");

#undef WRITE_FIELD_TO_FILE

    fclose(fd);
}

