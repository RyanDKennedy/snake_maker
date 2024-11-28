#include "map_create.hpp"
#include "collisions.hpp"
#include "common.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "map.hpp"
#include "text_box.hpp"

#include <cstdlib>

#include <stb_image.h>

inline const char *g_shader_bg_vertex_code =
"#version 330\n\
\n\
layout (location = 0) in vec3 a_pos;\n\
layout (location = 1) in vec2 a_tex_coord;\n\
uniform mat4 u_model;\n\
uniform mat4 u_vp;\n\
uniform int u_tile_x_amt;\n\
uniform int u_tile_y_amt;\n\
\n\
out vec2 v_tex_coord;\n\
\n\
void main()\n\
{\n\
    v_tex_coord = vec2(a_tex_coord.x * u_tile_x_amt, a_tex_coord.y * u_tile_y_amt);\n\
    gl_Position = u_vp * u_model * vec4(a_pos, 1.0f);\n\
}";

inline const char *g_shader_bg_fragment_code =
"#version 330\n\
\n\
out vec4 fragment_color;\n\
\n\
in vec2 v_tex_coord;\n\
\n\
uniform sampler2D u_tex;\n\
\n\
void main()\n\
{\n\
    fragment_color = texture(u_tex, v_tex_coord);\n\
}";



MapCreateCtx* map_create_start(GenericCtx *generic_ctx)
{
    MapCreateCtx *ctx = (MapCreateCtx*)calloc(1, sizeof(MapCreateCtx));

    // board map view
    ctx->view_buttons[BOARD_MAP_VIEW] = create_button("board map", 180, 40, Vec2i{10, 700},
						      Vec3i{50, 50, 50}, Vec3i{200, 200, 0}, 8, 2);
    memcpy(&ctx->view_buttons[BOARD_MAP_VIEW].bg_hover_color,
	   &ctx->view_buttons[BOARD_MAP_VIEW].fg_color,sizeof(Vec3i));
    memcpy(&ctx->view_buttons[BOARD_MAP_VIEW].fg_hover_color,
	   &ctx->view_buttons[BOARD_MAP_VIEW].bg_color, sizeof(Vec3i));

    // tile manager view
    ctx->view_buttons[TILE_MANAGER_VIEW] = create_button("tile manager", 210, 40, Vec2i{200, 700},
							 Vec3i{50, 50, 50}, Vec3i{200, 200, 0}, 8, 2);
    memcpy(&ctx->view_buttons[TILE_MANAGER_VIEW].bg_hover_color,
	   &ctx->view_buttons[TILE_MANAGER_VIEW].fg_color,sizeof(Vec3i));
    memcpy(&ctx->view_buttons[TILE_MANAGER_VIEW].fg_hover_color,
	   &ctx->view_buttons[TILE_MANAGER_VIEW].bg_color,sizeof(Vec3i));

    // collision map view
    ctx->view_buttons[COLLISION_MAP_VIEW] = create_button("collision map", 210, 40, Vec2i{10, 650},
							  Vec3i{50, 50, 50}, Vec3i{200, 200, 0}, 8, 2);
    memcpy(&ctx->view_buttons[COLLISION_MAP_VIEW].bg_hover_color,
	   &ctx->view_buttons[COLLISION_MAP_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[COLLISION_MAP_VIEW].fg_hover_color,
	   &ctx->view_buttons[COLLISION_MAP_VIEW].bg_color, sizeof(Vec3i));

    // settings view
    ctx->view_buttons[SETTINGS_VIEW] = create_button("settings", 180, 40, Vec2i{230, 650},
						     Vec3i{50, 50, 50}, Vec3i{200, 200, 0}, 8, 2);
    memcpy(&ctx->view_buttons[SETTINGS_VIEW].bg_hover_color,
	   &ctx->view_buttons[SETTINGS_VIEW].fg_color, sizeof(Vec3i));
    memcpy(&ctx->view_buttons[SETTINGS_VIEW].fg_hover_color,
	   &ctx->view_buttons[SETTINGS_VIEW].bg_color, sizeof(Vec3i));

    ctx->current_view = BOARD_MAP_VIEW;

    // Selector
    ctx->selector = text_box_create(21, Vec2i{420, 704}, 8, 2);
    ctx->selector_save_btn = create_button("save", 175, 40, Vec2i{420, 650}, Vec3i{50, 50, 50},
					   Vec3i{0x19, 0xd3, 0x13}, 8, 2);
    memcpy(&ctx->selector_save_btn.bg_hover_color, &ctx->selector_save_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->selector_save_btn.fg_hover_color, &ctx->selector_save_btn.bg_color, sizeof(Vec3i));
    ctx->selector_load_btn = create_button("load", 175, 40, Vec2i{605, 650},
					   Vec3i{50, 50, 50}, Vec3i{0xff, 0x4f, 0x13}, 8, 2);
    memcpy(&ctx->selector_load_btn.bg_hover_color, &ctx->selector_load_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->selector_load_btn.fg_hover_color, &ctx->selector_load_btn.bg_color, sizeof(Vec3i));

    // Create empty map
    ctx->map_space_dimensions[0] = 600;
    ctx->map_space_dimensions[1] = 600;
    {
	ctx->map = (SnakeMap*)calloc(1, sizeof(SnakeMap));

	int map_width = 20;
	int map_height = 20;
	int map_tile_width = 16;
	int map_tile_height = 16;

	ctx->map->height = 20;
	ctx->map->width = 20;
	ctx->map->size = ctx->map->width * ctx->map->height;
	ctx->map->tile_width = map_tile_width;
	ctx->map->tile_height = map_tile_height;
	new(&ctx->map->tile_quad) Quad();
	new(&ctx->map->tile_shader) Shader(g_shader_vertex_code, g_shader_fragment_code);
	ctx->map->collision_map = (int*)calloc(ctx->map->size, sizeof(int));
	ctx->map->board_map = (int*)calloc(ctx->map->size, sizeof(int));
	for (int i = 0; i < ctx->map->size; ++i)
	{
	    ctx->map->board_map[i] = -1;
	    ctx->map->collision_map[i] = 0;
	}
	ctx->map->tiles_amt = 0;
    }

    // Background

    new (&ctx->bg_quad) Quad;
    ctx->bg_quad.m_position[2] = -11.f;

    new (&ctx->bg_shader) Shader(g_shader_bg_vertex_code, g_shader_bg_fragment_code);
    
    int pix_width = ctx->map_space_dimensions[0];
    int pix_height = ctx->map_space_dimensions[1];
    float aspect_ratio = (float)(ctx->map->width * ctx->map->tile_width) / (ctx->map->height * ctx->map->tile_height);
    if (aspect_ratio > 1)
    {
	float width = pix_width * 0.75;
	float height = width / aspect_ratio;

	ctx->bg_quad.m_scale[0] = width;
	ctx->bg_quad.m_scale[1] = height;
	ctx->bg_quad.m_position[0] = (pix_width - ctx->bg_quad.m_scale[0]) / 2;
	ctx->bg_quad.m_position[1] = (pix_height - ctx->bg_quad.m_scale[1]) / 2;
    }
    else
    {
	float height = pix_height * 0.75;
	float width = height * aspect_ratio;

	ctx->bg_quad.m_scale[0] = width;
	ctx->bg_quad.m_scale[1] = height;
	ctx->bg_quad.m_position[0] = (pix_width - ctx->bg_quad.m_scale[0]) / 2;
	ctx->bg_quad.m_position[1] = (pix_height - ctx->bg_quad.m_scale[1]) / 2;
    }
    ctx->bg_quad.update_model();
    
    // Create texture for background
    glGenTextures(1, &ctx->bg_tex);
    glBindTexture(GL_TEXTURE_2D, ctx->bg_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    char buf[256];
    snprintf(buf, 256, "%s%s", g_background_dir, "transparent.png");
    int w, h, ch;
    void *data = stbi_load(buf, &w, &h, &ch, 0);
    int format = (ch == 4)? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);


    // Board Map stuff
    ctx->selected_tile = -1;

    return ctx;
}

GameReturnCode map_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, MapCreateCtx *map_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;
    
    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }


    // Clicked top buttons
    if (generic_ctx->mouse_released)
    {
	// Select view buttons
	for (int i = 0; i < 4; ++i)
	{
	    bool hover = is_inside_collision_box(&map_create_ctx->view_buttons[i].col_box, generic_ctx->mouse_pos);
	    if (hover)
	    {
		map_create_ctx->current_view = i;
	    }
	}

	// Load
	if (is_inside_collision_box(&map_create_ctx->selector_load_btn.col_box, generic_ctx->mouse_pos))
	{
	    char buf[256];
	    snprintf(buf, 256, "%s%s%s", g_map_dir, map_create_ctx->selector.text, g_map_file_extension);

	    SnakeMap *result = snake_map_create(buf);
	    if (result != NULL)
	    {
		snake_map_destroy(map_create_ctx->map);
		map_create_ctx->map = result;

		int pix_width = map_create_ctx->map_space_dimensions[0];
		int pix_height = map_create_ctx->map_space_dimensions[1];
		float aspect_ratio = (float)(map_create_ctx->map->width * map_create_ctx->map->tile_width) / (map_create_ctx->map->height * map_create_ctx->map->tile_height);
		if (aspect_ratio > 1)
		{
		    float width = pix_width * 0.75;
		    float height = width / aspect_ratio;
		    
		    map_create_ctx->bg_quad.m_scale[0] = width;
		    map_create_ctx->bg_quad.m_scale[1] = height;
		    map_create_ctx->bg_quad.m_position[0] = (pix_width - map_create_ctx->bg_quad.m_scale[0]) / 2;
		    map_create_ctx->bg_quad.m_position[1] = (pix_height - map_create_ctx->bg_quad.m_scale[1]) / 2;
		}
		else
		{
		    float height = pix_height * 0.75;
		    float width = height * aspect_ratio;
		    
		    map_create_ctx->bg_quad.m_scale[0] = width;
		    map_create_ctx->bg_quad.m_scale[1] = height;
		    map_create_ctx->bg_quad.m_position[0] = (pix_width - map_create_ctx->bg_quad.m_scale[0]) / 2;
		    map_create_ctx->bg_quad.m_position[1] = (pix_height - map_create_ctx->bg_quad.m_scale[1]) / 2;
		}
		map_create_ctx->bg_quad.update_model();
		
	    }
	}

	// Save
	if (is_inside_collision_box(&map_create_ctx->selector_save_btn.col_box, generic_ctx->mouse_pos))
	{
	    printf("saving\n");	    
	}

    }

    // Draw selector
    {
	bool selector_selected = is_inside_collision_box(&map_create_ctx->selector.col_box,
							 generic_ctx->mouse_pos);
	if (selector_selected)
	    text_box_parse_key_list(&map_create_ctx->selector, generic_ctx->key_list);
	
	draw_text_box(pixel_map, &map_create_ctx->selector, selector_selected);
    }

    // Load and save
    {
	bool hover = is_inside_collision_box(&map_create_ctx->selector_load_btn.col_box, generic_ctx->mouse_pos);
	draw_button(pixel_map, &map_create_ctx->selector_load_btn, hover);
    }
    {
	bool hover = is_inside_collision_box(&map_create_ctx->selector_save_btn.col_box, generic_ctx->mouse_pos);
	draw_button(pixel_map, &map_create_ctx->selector_save_btn, hover);
    }

    // Draw view buttons
    for (int i = 0; i < 4; ++i)
    {
	bool selected = (map_create_ctx->current_view == i);
	draw_button(pixel_map, &map_create_ctx->view_buttons[i], selected);
    }

    switch (map_create_ctx->current_view)
    {
	case BOARD_MAP_VIEW:
	{

	    Vec2i bg_pos = {(int)map_create_ctx->bg_quad.m_position[0], (int)map_create_ctx->bg_quad.m_position[1]};

	    // Handle input
	    if (generic_ctx->mouse_clicked)
	    {
		int x = (generic_ctx->mouse_pos[0] - bg_pos[0]) / (map_create_ctx->bg_quad.m_scale[0]/map_create_ctx->map->width);
		int y = (generic_ctx->mouse_pos[1] - bg_pos[1]) / (map_create_ctx->bg_quad.m_scale[1]/map_create_ctx->map->height);

		if (x >= 0 && x < map_create_ctx->map->width && y >= 0 && y < map_create_ctx->map->height)
		{
		    map_create_ctx->map->board_map[y * map_create_ctx->map->width + x] = map_create_ctx->selected_tile;
		}
	    }
	    if (generic_ctx->mouse_right_clicked)
	    {
		int x = (generic_ctx->mouse_pos[0] - bg_pos[0]) / (map_create_ctx->bg_quad.m_scale[0]/map_create_ctx->map->width);
		int y = (generic_ctx->mouse_pos[1] - bg_pos[1]) / (map_create_ctx->bg_quad.m_scale[1]/map_create_ctx->map->height);

		if (x >= 0 && x < map_create_ctx->map->width && y >= 0 && y < map_create_ctx->map->height)
		{
		    map_create_ctx->selected_tile = map_create_ctx->map->board_map[y * map_create_ctx->map->width + x];
		}
	    }	    

	    // Draw transparent bg
	    glBindTexture(GL_TEXTURE_2D, map_create_ctx->bg_tex);
	    map_create_ctx->bg_shader.use();
	    glUniform1i(glGetUniformLocation(map_create_ctx->bg_shader.m_program, "u_tex"), 0);
	    glUniformMatrix4fv(glGetUniformLocation(map_create_ctx-> bg_shader.m_program, "u_vp"), 1, GL_FALSE, glm::value_ptr(generic_ctx->vp_matrix));
	    glUniformMatrix4fv(glGetUniformLocation(map_create_ctx->bg_shader.m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(map_create_ctx->bg_quad.m_model_matrix));
	    glUniform1i(glGetUniformLocation(map_create_ctx->bg_shader.m_program, "u_tile_x_amt"), map_create_ctx->map->width);
	    glUniform1i(glGetUniformLocation(map_create_ctx->bg_shader.m_program, "u_tile_y_amt"), map_create_ctx->map->height);
	    map_create_ctx->bg_quad.draw();

	    // Draw snake map
	    draw_snake_map(map_create_ctx->map, map_create_ctx->bg_quad.m_scale[0], map_create_ctx->bg_quad.m_scale[1], bg_pos, generic_ctx->vp_matrix);

	    draw_rectangle(pixel_map, map_create_ctx->bg_quad.m_scale[0], map_create_ctx->bg_quad.m_scale[1], bg_pos, Vec3i{0, 0, 0}, 0);



	    break;
	}

	case COLLISION_MAP_VIEW:
	{

	    break;
	}

	case TILE_MANAGER_VIEW:
	{


	    break;
	}

	case SETTINGS_VIEW:
	{

	    break;
	}

    }

    return return_code;
}

void map_create_end(MapCreateCtx *map_create_ctx)
{
    glDeleteTextures(1, &map_create_ctx->bg_tex);
    map_create_ctx->bg_quad.~Quad();
    map_create_ctx->bg_shader.~Shader();

    text_box_destroy(&map_create_ctx->selector);
    snake_map_destroy(map_create_ctx->map);
    free(map_create_ctx);
}
