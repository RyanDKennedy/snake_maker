#pragma once

#include "common.hpp"
#include "context.hpp"
#include "pixel_map.hpp"
#include "button.hpp"
#include "text_box.hpp"
#include "map.hpp"
#include "quad.hpp"
#include "shader.hpp"

#define BOARD_MAP_VIEW 0
#define COLLISION_MAP_VIEW 1
#define TILE_MANAGER_VIEW 2
#define SETTINGS_VIEW 3

struct TileButton
{
    CollisionBox col_box;
    int tile;
};

TileButton tile_button_create(int tile, int width, int height, Vec2i pos);
void draw_tile_button(TileButton *tile_button, GLuint tex, Quad *quad, Shader *shader, glm::mat4 vp, GLuint bg);

struct MapCreateCtx
{
    int current_view;
    
    Button view_buttons[4];

    TextBox selector;
    Button selector_load_btn;
    Button selector_save_btn;

    SnakeMap *map;

    // Transparent bg
    Quad bg_quad;
    Shader bg_shader;
    GLuint bg_tex;

    // Board map stuff
    int selected_tile;
    Vec2i map_space_dimensions;
    Button next_tile_line_btn;
    Button prev_tile_line_btn;
    int tiles_line_length;
    int current_tiles_line;
    Vec2i tiles_pos;
    Vec2i tiles_dimensions;
    TileButton selected_tile_button;
    TileButton *tile_buttons;
    int tile_buttons_amt;
    Quad tile_quad;
    Shader tile_shader;
};


MapCreateCtx* map_create_start(GenericCtx *generic_ctx);

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx);

void map_create_end(MapCreateCtx *map_create_ctx);

void map_create_load_map(MapCreateCtx *map_create_ctx);
