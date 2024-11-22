#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "button.hpp"
#include "drawing.hpp"
#include "collisions.hpp"
#include "increment_value.hpp"


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

