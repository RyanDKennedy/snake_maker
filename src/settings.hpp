#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"

struct Settings
{
    int starting_speed;
};

struct SettingsCtx
{
    Settings settings;

};

Settings settings_from_file(const char *path);

SettingsCtx* settings_start(GenericCtx *generic_ctx);

GameReturnCode settings_run(PixelMap *pixel_map, GenericCtx *generic_ctx, SettingsCtx *settings_ctx);

void settings_end(SettingsCtx *settings_ctx);
