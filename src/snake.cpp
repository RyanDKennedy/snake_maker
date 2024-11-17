#include "snake.hpp"

#include <cstdlib>

SnakeCtx* snake_start(GenericCtx *generic_ctx, const char *map_name)
{
    SnakeCtx *snake_ctx = (SnakeCtx*)calloc(1, sizeof(SnakeCtx));

    return snake_ctx;
}

void snake_run(GenericCtx *generic_ctx, SnakeCtx *snake_ctx)
{

}
