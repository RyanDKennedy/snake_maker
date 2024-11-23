#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "increment_value.hpp"
#include "slider.hpp"

struct TextBox
{
    Vec2i pos;
    int width;
    int height;
    char *text;
};

struct TileCreateCtx
{
    static const int tile_width = 10;
    static const int tile_height = 10;
    static const int tile_size = tile_width * tile_height;

    RGBPixel current_color;

    Slider red_slider;
    Slider green_slider;
    Slider blue_slider;

    RGBPixel tile[tile_size];

    PixelMap tile_pixmap;

};

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx);

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx);

void tile_create_end(GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx);