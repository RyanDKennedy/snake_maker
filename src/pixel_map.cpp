#include "pixel_map.hpp"

#include <cstdlib>
#include <cstring>

PixelMap pixel_map_create(int width, int height)
{
    PixelMap result;
    result.width = width;
    result.height = height;
    result.size = width * height;
    result.data = (RGBPixel*)calloc(result.size, sizeof(RGBPixel));
    memset(result.data, 0, sizeof(RGBPixel) * result.size);
    return result;
}

void pixel_map_destroy(PixelMap *pixel_map)
{
    pixel_map->width = 0;
    pixel_map->height = 0;    
    pixel_map->size = 0;    
    free(pixel_map->data);
    pixel_map->data = NULL;
}
