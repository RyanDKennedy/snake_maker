#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "slider.hpp"
#include "text_box.hpp"
#include "button.hpp"

struct TileCreateCtx
{
     int tile_width;
     int tile_height;
    int tile_size;
    RGBPixel *tile;

    RGBPixel current_color;
    
    static const int old_colors_amt = 10;
    RGBPixel old_colors[old_colors_amt];

    Slider red_slider;
    Slider green_slider;
    Slider blue_slider;

    PixelMap tile_pixmap;

    TextBox text;
    Button save_btn;
    Button load_btn;

};

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx);

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx);

void tile_create_end(TileCreateCtx *tile_create_ctx);
