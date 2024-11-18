#pragma once

#include "game_state.hpp"
#include "math_types.hpp"

struct GenericCtx
{
    bool mouse_clicked;
    Vec2i mouse_pos;
    double mouse_scroll;
    double delta_time;
    GameState game_state;
    struct
    {
	bool w;
	bool a;
	bool s;
	bool d;
	bool space;
	bool enter;
	bool backspace;
    } keyboard;
    char last_pressed_key;

};
