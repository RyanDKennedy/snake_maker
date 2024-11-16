#pragma once

#include "pixel.hpp"

struct PixelMap
{
    int height;
    int width;
    int size;
    RGBPixel *data;
};

PixelMap pixel_map_create(int width, int height);
void pixel_map_destroy(PixelMap *pixel_map);

