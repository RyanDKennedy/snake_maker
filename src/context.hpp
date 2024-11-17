#pragma once

#include "game_state.hpp"
#include "math_types.hpp"

struct GenericCtx
{
    bool mouse_clicked;
    Vec2i mouse_pos;
    double delta_time;
    GameState game_state;
};
