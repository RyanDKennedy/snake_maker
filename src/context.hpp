#pragma once

#include "game_state.hpp"
#include "math_types.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Settings
{
    int starting_speed;
    int starting_size;

    int tile_create_width;
    int tile_create_height;

    int texture_filtering;
};

struct GenericCtx
{
    int pix_width;
    int pix_height;

    bool mouse_clicked;
    bool mouse_right_clicked;
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
	bool escape;
    } keyboard;
    char last_pressed_key;
    
    Settings settings;

    char *key_list;

    glm::mat4 vp_matrix;
};
