#include "settings.hpp"
#include "collisions.hpp"
#include "game_state.hpp"

#include <cstdlib>
#include <string>
#include <cstring>
#include "utils.hpp"

#define STARTING_SPEED_INDEX 0
#define STARTING_SIZE_INDEX 1

SettingsCtx* settings_start(GenericCtx *generic_ctx)
{
    SettingsCtx *ctx = (SettingsCtx*)calloc(1, sizeof(SettingsCtx));
    ctx->in_prog_settings = generic_ctx->settings;

    ctx->values_amt = 2;
    ctx->values = (IncrementValue*)calloc(ctx->values_amt, sizeof(IncrementValue));

    IncrementValue *starting_speed = &ctx->values[STARTING_SPEED_INDEX];
    IncrementValue *starting_size = &ctx->values[STARTING_SIZE_INDEX];

    *starting_speed = create_increment_value(generic_ctx->settings.starting_speed, "starting_speed", Vec2i{200, 600}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});

    *starting_size = create_increment_value(generic_ctx->settings.starting_size, "starting_size", Vec2i{200, 550}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});

    return ctx;
}

GameReturnCode settings_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SettingsCtx *settings_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.backspace)
    {
	return_code = GameReturnCode::goto_menu;
    }

    // Update values
    update_increment_value(&settings_ctx->values[STARTING_SPEED_INDEX], generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);
    update_increment_value(&settings_ctx->values[STARTING_SIZE_INDEX], generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);

    // Draw
    draw_increment_value(pixel_map, &settings_ctx->values[STARTING_SPEED_INDEX], generic_ctx->mouse_pos);
    draw_increment_value(pixel_map, &settings_ctx->values[STARTING_SIZE_INDEX], generic_ctx->mouse_pos);

    // Load values into temporary settings
    settings_ctx->in_prog_settings.starting_speed = settings_ctx->values[STARTING_SPEED_INDEX].value;
    settings_ctx->in_prog_settings.starting_size = settings_ctx->values[STARTING_SIZE_INDEX].value;

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

#undef WRITE_FIELD_TO_FILE

    fclose(fd);
}

IncrementValue create_increment_value(int initial_value, const char *name, Vec2i pos, Vec3i bg_color, Vec3i fg_color)
{
    IncrementValue value;

    const int font_size = 8;
    const int font_mag = 2;
    const int text_width = font_size * font_mag * (strlen(name) + 1);

    const int height = font_size * font_mag * 2;

    Vec2i down_pos;
    down_pos[0] = pos[0] + text_width;
    down_pos[1] = pos[1];

    Vec2i up_pos;
    up_pos[0] = down_pos[0] + font_size * font_mag * 2;
    up_pos[1] = down_pos[1];

    value.value = initial_value;
    value.text = create_button(name, text_width, height, pos, bg_color, fg_color, font_size, font_mag);
    value.down = create_button("v", font_size * font_mag * 2, height, down_pos, bg_color, fg_color, font_size, font_mag);
    memcpy(&value.down.fg_hover_color, bg_color, sizeof(Vec3i));
    memcpy(&value.down.bg_hover_color, fg_color, sizeof(Vec3i));

    value.up = create_button("^", font_size * font_mag * 2, height, up_pos, bg_color, fg_color, font_size, font_mag);
    memcpy(&value.up.fg_hover_color, bg_color, sizeof(Vec3i));
    memcpy(&value.up.bg_hover_color, fg_color, sizeof(Vec3i));

    value.ready = true;

    return value;
}

void draw_increment_value(PixelMap *pixel_map, IncrementValue *val, Vec2i mouse_pos)
{
    bool up_hover = is_inside_collision_box(&val->up.col_box, mouse_pos);
    bool down_hover = is_inside_collision_box(&val->down.col_box, mouse_pos);

    draw_button(pixel_map, &val->text, false);
    draw_button(pixel_map, &val->down, down_hover);
    draw_button(pixel_map, &val->up, up_hover);

    Vec2i val_pos;
    val_pos[0] = val->up.col_box.top_right[0];
    val_pos[1] = val->up.col_box.bottom_left[1];

    char buf[256];
    snprintf(buf, 256, " %d", val->value);
    draw_sentence(pixel_map, buf, 8, 3, val_pos, Vec3i{255, 255, 255});
}

void update_increment_value(IncrementValue *val, Vec2i mouse_pos, bool mouse_clicked, bool mouse_released)
{
    if (mouse_released == true)
    {
	val->ready = true;
    }

    if (mouse_clicked && val->ready)
    {
	bool up_hover = is_inside_collision_box(&val->up.col_box, mouse_pos);
	bool down_hover = is_inside_collision_box(&val->down.col_box, mouse_pos);

	if (up_hover)
	    ++val->value;
	
	if (down_hover && val->value > 0)
	    --val->value;

	val->ready = false;
    }
}