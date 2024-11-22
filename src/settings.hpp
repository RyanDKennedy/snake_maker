#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "button.hpp"
#include "drawing.hpp"
#include "collisions.hpp"


struct IncrementValue
{
    Button text;
    Button up;
    Button down;
    int value;
    bool ready;
};

struct SettingsCtx
{
    Settings in_prog_settings;

    int values_amt;
    IncrementValue *values;
};


SettingsCtx* settings_start(GenericCtx *generic_ctx);

GameReturnCode settings_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SettingsCtx *settings_ctx);

void settings_end(GenericCtx *generic_ctx, SettingsCtx *settings_ctx);

Settings settings_from_file(const char *path);
void settings_write_to_file(const char *path, Settings *settings);


IncrementValue create_increment_value(int initial_value, const char *name, Vec2i pos, Vec3i bg_color, Vec3i fg_color);
void draw_increment_value(PixelMap *pixel_map, IncrementValue *val, Vec2i mouse_pos);
void update_increment_value(IncrementValue *val, Vec2i mouse_pos, bool mouse_clicked, bool mouse_released);


