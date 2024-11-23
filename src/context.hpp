#pragma once

#include "game_state.hpp"
#include "math_types.hpp"

struct Settings
{
    int starting_speed;
    int starting_size;
};

struct GenericCtx
{
    bool mouse_clicked;
    bool mouse_released;
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
    
    Settings settings;

    char *key_list;
};
