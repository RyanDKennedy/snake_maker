#include "map_create.hpp"
#include "button.hpp"
#include "collisions.hpp"
#include "common.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "increment_value.hpp"
#include "map.hpp"
#include "pixel_map.hpp"
#include "text_box.hpp"

#include <cstdlib>
#include <filesystem>

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

    {

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
	
	{
	    char buf[256];
	    snprintf(buf, 256, "%s%s", g_resources_dir, "transparent.png");
	    int w, h, ch;
	    void *data = stbi_load(buf, &w, &h, &ch, 0);
	    int format = (ch == 4)? GL_RGBA : GL_RGB;
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	    stbi_image_free(data);
	}
    }
    
    // Board Map stuff

    {
	ctx->selected_tile = -1;
	ctx->selected_tile_button = tile_button_create(0, 100, 100, Vec2i{700, 400});
	new(&ctx->tile_quad) Quad();
	ctx->tile_quad.m_position[2] = -9.f;
	new(&ctx->tile_shader) Shader(g_shader_vertex_code, g_shader_fragment_code);
	
	// Tiles
	
	{
	    float tile_aspect_ratio = (float)ctx->map->tile_width / ctx->map->tile_height;
	    if (tile_aspect_ratio > 1)
	    {
		float width = 50;
		float height = width / tile_aspect_ratio;
		
		ctx->tiles_dimensions[0] = width;
		ctx->tiles_dimensions[1] = height;
	    }
	    else
	    {
		float height = 50;
		float width = height * tile_aspect_ratio;
		
		ctx->tiles_dimensions[0] = width;
		ctx->tiles_dimensions[1] = height;
	    }
	    ctx->tiles_pos[0] = 550;
	    ctx->tiles_pos[1] = 550;
	    
	    ctx->tiles_line_length = (ctx->tiles_pos[1]) / ctx->tiles_dimensions[1];
	    ctx->current_tiles_line = 0;
	    
	    int tile_width = ctx->tiles_dimensions[0];
	    int tile_height = ctx->tiles_dimensions[1];
	    Vec2i pos;
	    pos[0] = ctx->tiles_pos[0];
	    pos[1] = ctx->tiles_pos[1];
	    
	    ctx->tile_buttons_amt = ctx->map->tiles_amt + 1;
	    ctx->tile_buttons = (TileButton*)calloc(ctx->tile_buttons_amt, sizeof(TileButton));
	    ctx->tile_buttons[0] = tile_button_create(-1, tile_width, tile_height, pos);
	    for (int i = 1; i < ctx->tile_buttons_amt; ++i)
	    {
		ctx->tile_buttons[i] = tile_button_create(i - 1, tile_width, tile_height, Vec2i{pos[0] , pos[1] - i * tile_height + (i / ctx->tiles_line_length * ctx->tiles_line_length * ctx->tiles_dimensions[1])});
	    }
	}
	
	ctx->next_tile_line_btn = create_button(">", 50, 50, Vec2i{460, 550},
						Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
	memcpy(&ctx->next_tile_line_btn.bg_hover_color, &ctx->next_tile_line_btn.fg_color, sizeof(Vec3i));
	memcpy(&ctx->next_tile_line_btn.fg_hover_color, &ctx->next_tile_line_btn.bg_color, sizeof(Vec3i));
	
	ctx->prev_tile_line_btn = create_button("<", 50, 50, Vec2i{400, 550},
						Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
	memcpy(&ctx->prev_tile_line_btn.bg_hover_color, &ctx->prev_tile_line_btn.fg_color, sizeof(Vec3i));
	memcpy(&ctx->prev_tile_line_btn.fg_hover_color, &ctx->prev_tile_line_btn.bg_color, sizeof(Vec3i));
    }

    // Collision map stuff

    {
	PixelMap pixel_map = pixel_map_create(16, 16);

	char buf[256];

	snprintf(buf, 256, "%s%s%s", g_resources_dir, "check", g_tile_file_extension);
	load_tile_from_file(buf, &pixel_map);

	glGenTextures(1, &ctx->check_tex);
	glBindTexture(GL_TEXTURE_2D, ctx->check_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_map.data);
	
	snprintf(buf, 256, "%s%s%s", g_resources_dir, "cross", g_tile_file_extension);
	load_tile_from_file(buf, &pixel_map);

	glGenTextures(1, &ctx->cross_tex);
	glBindTexture(GL_TEXTURE_2D, ctx->cross_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_map.data);


	pixel_map_destroy(&pixel_map);
    }

    // tile manager

    {
	// Get amount of tile files
	std::string path = g_tile_dir;
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
	    if (entry.is_regular_file())
	    {
		std::string path_name_str = entry.path().string();
		const char* path_name = path_name_str.c_str();
		if (strlen(path_name) >= strlen(g_tile_file_extension) && memcmp(path_name + (strlen(path_name) - strlen(g_tile_file_extension)), g_tile_file_extension, strlen(g_tile_file_extension)) == 0)
		{
		    int height;
		    int width;
		    get_dimensions_of_tile_file(path_name, &width, &height);
		    if (height == ctx->map->tile_height && width == ctx->map->tile_width)
			++ctx->available_tiles_amt;
		}
	    }
	}
	
	// Create the buttons for the tiles
	int num = 0;
	ctx->available_tiles = (Button*)calloc(ctx->available_tiles_amt, sizeof(Button));
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
	    if (entry.is_regular_file())
	    {
		std::string path_name_str = entry.path().string();
		const char *path_name = path_name_str.c_str();
		if (strlen(path_name) >= strlen(g_tile_file_extension) && memcmp(path_name + (strlen(path_name) - strlen(g_tile_file_extension)), g_tile_file_extension, strlen(g_tile_file_extension)) == 0)
		{
		    
		    int height;
		    int width;
		    get_dimensions_of_tile_file(path_name, &width, &height);
		    if (height == ctx->map->tile_height && width == ctx->map->tile_width)
		    {
			const char *path = strrchr(path_name, '/') + 1;
			const int path_size = strlen(path) - 5;
			
			char *new_path = (char*)calloc(path_size + 1, sizeof(char));
			memcpy(new_path, path, path_size);
			new_path[path_size] = '\0';
			
			ctx->available_tiles[num] = create_button((const char*)new_path, 200, 30, Vec2i{0, 0}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 1);
			++num;
		    }
		}
	    }
	}
	
	// highlight in use tiles
	// set all to red
	for (int tile_num = 0; tile_num < ctx->available_tiles_amt; ++tile_num)
	{
	    memcpy(ctx->available_tiles[tile_num].bg_color, Vec3i{255, 0, 0}, sizeof(Vec3i));
	}
	
	// search for green
	for (int i = 0; i < ctx->map->tiles_amt; ++i)
	{
	    const char *tile_name = ctx->map->tile_names[i];
	    
	    for (int tile_num = 0; tile_num < ctx->available_tiles_amt; ++tile_num)
	    {
		if (strcmp(tile_name, ctx->available_tiles[tile_num].text) == 0)
		{
		    memcpy(ctx->available_tiles[tile_num].bg_color, Vec3i{0, 150, 0}, sizeof(Vec3i));
		    break;
		}
	    }
	    
	}

	// Create tile page navigation buttons

	ctx->tile_page_next_btn = create_button(">", 50, 50, Vec2i{90, 575},
						Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
	memcpy(&ctx->tile_page_next_btn.bg_hover_color, &ctx->tile_page_next_btn.fg_color, sizeof(Vec3i));
	memcpy(&ctx->tile_page_next_btn.fg_hover_color, &ctx->tile_page_next_btn.bg_color, sizeof(Vec3i));
	
	ctx->tile_page_prev_btn = create_button("<", 50, 50, Vec2i{30, 575},
						Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 2);
	memcpy(&ctx->tile_page_prev_btn.bg_hover_color, &ctx->tile_page_prev_btn.fg_color, sizeof(Vec3i));
	memcpy(&ctx->tile_page_prev_btn.fg_hover_color, &ctx->tile_page_prev_btn.bg_color, sizeof(Vec3i));
	


    }

    // settings stuff
    {
	ctx->settings_save_btn = create_button("save", 200, 50, Vec2i{300, 550}, Vec3i{50, 200, 50}, Vec3i{255, 255, 255}, 8, 2);
	memcpy(&ctx->settings_save_btn.bg_hover_color, &ctx->settings_save_btn.fg_color, sizeof(Vec3i));
	memcpy(&ctx->settings_save_btn.fg_hover_color, &ctx->settings_save_btn.bg_color, sizeof(Vec3i));

	ctx->settings_width = create_increment_value(ctx->map->width, "width", Vec2i{0, 400}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
	ctx->settings_height = create_increment_value(ctx->map->height, "height", Vec2i{0, 350}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
	ctx->settings_tile_width = create_increment_value(ctx->map->tile_width, "tile_width", Vec2i{0, 300}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});
	ctx->settings_tile_height = create_increment_value(ctx->map->tile_height, "tile_height", Vec2i{0, 250}, Vec3i{250, 0, 0}, Vec3i{255, 255, 255});	
    }
    
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
	    map_create_load_map(map_create_ctx);
	}

	// Save
	if (is_inside_collision_box(&map_create_ctx->selector_save_btn.col_box, generic_ctx->mouse_pos))
	{
	    char buf[256];
	    snprintf(buf, 256, "%s%s%s", g_map_dir, map_create_ctx->selector.text, g_map_file_extension);

	    snake_map_write_to_file(map_create_ctx->map, buf);
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
	    if (generic_ctx->mouse_released)
	    {
		int starting_index = map_create_ctx->tiles_line_length * map_create_ctx->current_tiles_line;
		int ending_index = (map_create_ctx->tile_buttons_amt < starting_index + map_create_ctx->tiles_line_length)? map_create_ctx->tile_buttons_amt : starting_index + map_create_ctx->tiles_line_length;
		for (int i = starting_index; i < ending_index; ++i)
		{
		    if (is_inside_collision_box(&map_create_ctx->tile_buttons[i].col_box, generic_ctx->mouse_pos))
		    {
			map_create_ctx->selected_tile = map_create_ctx->tile_buttons[i].tile;
		    }

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


	    // draw/change tile lines
	    {
		bool hover = is_inside_collision_box(&map_create_ctx->next_tile_line_btn.col_box, generic_ctx->mouse_pos);
		draw_button(pixel_map, &map_create_ctx->next_tile_line_btn, hover);

		if (hover && generic_ctx->mouse_released)
		{
		    ++map_create_ctx->current_tiles_line;
		}
		
		hover = is_inside_collision_box(&map_create_ctx->prev_tile_line_btn.col_box, generic_ctx->mouse_pos);
		draw_button(pixel_map, &map_create_ctx->prev_tile_line_btn, hover);

		if (hover && generic_ctx->mouse_released)
		{
		    --map_create_ctx->current_tiles_line;
		}

		if (map_create_ctx->current_tiles_line < 0)
		    map_create_ctx->current_tiles_line = 0;

		if (map_create_ctx->map->tiles_amt / map_create_ctx->tiles_line_length < map_create_ctx->current_tiles_line)
		    map_create_ctx->current_tiles_line = map_create_ctx->map->tiles_amt / map_create_ctx->tiles_line_length;

	    }
	    
	    // Draw tiles
	    {
		
		int starting_index = map_create_ctx->tiles_line_length * map_create_ctx->current_tiles_line;
		int ending_index = (map_create_ctx->tile_buttons_amt < starting_index + map_create_ctx->tiles_line_length)? map_create_ctx->tile_buttons_amt : starting_index + map_create_ctx->tiles_line_length;

		// draw available tiles
		for (int i = starting_index; i < ending_index; ++i)
		{
		    TileButton *but = &map_create_ctx->tile_buttons[i];
		    int tile = but->tile;
		    
		    Vec3i color;
		    if (tile == map_create_ctx->selected_tile)
		    {
			color[0] = 255;
			color[1] = 255;
			color[2] = 0;
		    }
		    else
		    {
			color[0] = 255;
			color[1] = 255;
			color[2] = 255;
		    }


		    GLuint tex;
		    if (tile != -1)
		    {
			tex = map_create_ctx->map->tile_textures[tile];

			const int size = (generic_ctx->pix_height - (map_create_ctx->tiles_pos[0] + map_create_ctx->tiles_dimensions[0])) / 8;
			char text[256];
			snprintf(text, size, "%s", map_create_ctx->map->tile_names[tile]);
			draw_sentence(pixel_map, text, 8, 1, Vec2i{but->col_box.top_right[0] + 4, but->col_box.bottom_left[1] + map_create_ctx->tiles_dimensions[1] / 2 - 4}, color);
		    }
		    else
		    {
			tex = map_create_ctx->bg_tex;
			draw_sentence(pixel_map, "none", 8, 1, Vec2i{but->col_box.top_right[0] + 4, but->col_box.bottom_left[1] + map_create_ctx->tiles_dimensions[1] / 2 - 4}, color);

		    }

		    draw_tile_button(&map_create_ctx->tile_buttons[i], tex, &map_create_ctx->tile_quad, &map_create_ctx->tile_shader, generic_ctx->vp_matrix, map_create_ctx->bg_tex);
		    
		}


	    }

	    break;
	}

	case COLLISION_MAP_VIEW:
	{

	    Vec2i bg_pos = {(int)map_create_ctx->bg_quad.m_position[0], (int)map_create_ctx->bg_quad.m_position[1]};

	    if (generic_ctx->mouse_right_clicked)
	    {
		int x = (generic_ctx->mouse_pos[0] - bg_pos[0]) / (map_create_ctx->bg_quad.m_scale[0]/map_create_ctx->map->width);
		int y = (generic_ctx->mouse_pos[1] - bg_pos[1]) / (map_create_ctx->bg_quad.m_scale[1]/map_create_ctx->map->height);

		if (x >= 0 && x < map_create_ctx->map->width && y >= 0 && y < map_create_ctx->map->height)
		{
		    map_create_ctx->map->collision_map[y * map_create_ctx->map->width + x] = 0;
		}
	    }
	    if (generic_ctx->mouse_clicked)
	    {
		int x = (generic_ctx->mouse_pos[0] - bg_pos[0]) / (map_create_ctx->bg_quad.m_scale[0]/map_create_ctx->map->width);
		int y = (generic_ctx->mouse_pos[1] - bg_pos[1]) / (map_create_ctx->bg_quad.m_scale[1]/map_create_ctx->map->height);

		if (x >= 0 && x < map_create_ctx->map->width && y >= 0 && y < map_create_ctx->map->height)
		{
		    map_create_ctx->map->collision_map[y * map_create_ctx->map->width + x] = 1;
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

	    // draw the board map
	    draw_rectangle(pixel_map, map_create_ctx->bg_quad.m_scale[0], map_create_ctx->bg_quad.m_scale[1], bg_pos, Vec3i{0, 0, 0}, 0);
	    draw_snake_map(map_create_ctx->map, map_create_ctx->bg_quad.m_scale[0], map_create_ctx->bg_quad.m_scale[1], bg_pos, generic_ctx->vp_matrix);


	    // draw the pixel map on top
	    {
		map_create_ctx->map->tile_shader.use();
		glUniform1i(glGetUniformLocation(map_create_ctx->map->tile_shader.m_program, "u_tex"), 0);
		glUniformMatrix4fv(glGetUniformLocation(map_create_ctx->map->tile_shader.m_program, "u_vp"), 1, GL_FALSE, glm::value_ptr(generic_ctx->vp_matrix));
		glActiveTexture(GL_TEXTURE0);
		
		float tile_width = (float)map_create_ctx->bg_quad.m_scale[0] / map_create_ctx->map->width;
		float tile_height = (float)map_create_ctx->bg_quad.m_scale[1] / map_create_ctx->map->height;
		
		map_create_ctx->map->tile_quad.m_scale[0] = tile_width;
		map_create_ctx->map->tile_quad.m_scale[1] = tile_height;
		map_create_ctx->map->tile_quad.m_position[2] = -9.f;
		
		for (int y = 0; y < map_create_ctx->map->height; ++y)
		{
		    for (int x = 0; x < map_create_ctx->map->width; ++x)
		    {
			GLuint tex;
			if (map_create_ctx->map->collision_map[x + y * map_create_ctx->map->width] == 0)
			{
			    tex = map_create_ctx->cross_tex;
			}
			else if (map_create_ctx->map->collision_map[x + y * map_create_ctx->map->width] == 1)
			{
			    tex = map_create_ctx->check_tex;
			}
			else
			{
			    continue;
			}
			
			glBindTexture(GL_TEXTURE_2D, tex);
			map_create_ctx->map->tile_quad.m_position[0] = bg_pos[0] + tile_width * x;
			map_create_ctx->map->tile_quad.m_position[1] = bg_pos[1] + tile_height * y;
			
			map_create_ctx->map->tile_quad.update_model();
			glUniformMatrix4fv(glGetUniformLocation(map_create_ctx->map->tile_shader.m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(map_create_ctx->map->tile_quad.m_model_matrix));
			
			map_create_ctx->map->tile_quad.draw();
		    }
		}
		
	    }
	    break;
	}

	case TILE_MANAGER_VIEW:
	{

	    // Draw the nav buttons and handle their clicked actions
	    bool hover = is_inside_collision_box(&map_create_ctx->tile_page_next_btn.col_box, generic_ctx->mouse_pos);
	    draw_button(pixel_map, &map_create_ctx->tile_page_next_btn, hover);
	    if (hover && generic_ctx->mouse_released)
		++map_create_ctx->current_tile_page;
	    
	    hover = is_inside_collision_box(&map_create_ctx->tile_page_prev_btn.col_box, generic_ctx->mouse_pos);
	    draw_button(pixel_map, &map_create_ctx->tile_page_prev_btn, hover);
	    if (hover && generic_ctx->mouse_released)
		--map_create_ctx->current_tile_page;

	    if (map_create_ctx->current_tile_page >= map_create_ctx->tile_page_amt)
		map_create_ctx->current_tile_page = map_create_ctx->tile_page_amt - 1;

	    if (map_create_ctx->current_tile_page < 0)
		map_create_ctx->current_tile_page = 0;


	    // Draw the tile buttons
	    map_create_ctx->tile_page_width = 3;
	    map_create_ctx->tile_page_height = 11;
	    
	    map_create_ctx->tile_page_size = map_create_ctx->tile_page_width * map_create_ctx->tile_page_height;
	    map_create_ctx->tile_page_amt = map_create_ctx->available_tiles_amt / map_create_ctx->tile_page_size + 1;
	    
	    int end_indice = (map_create_ctx->current_tile_page + 1) * map_create_ctx->tile_page_size;
	    if (end_indice > map_create_ctx->available_tiles_amt)
		end_indice = map_create_ctx->available_tiles_amt;

	    int x = 0;
	    int y = 0;
	    for (int i = map_create_ctx->current_tile_page * map_create_ctx->tile_page_size; i < end_indice; ++i)
	    {
		Button *btn = map_create_ctx->available_tiles + i;

		move_button(btn, Vec2i{ 20 + x * 250, 20 + y * 50}, false);

		if (generic_ctx->mouse_released && is_inside_collision_box(&btn->col_box, generic_ctx->mouse_pos))
		{
		    bool is_loaded = memcmp(btn->bg_color, Vec3i{0, 150, 0}, sizeof(Vec3i)) == 0;
		    if (is_loaded)
		    {
			int index = 0;
			for (; index < map_create_ctx->map->tiles_amt; ++index)
			{
			    if (strcmp(btn->text, map_create_ctx->map->tile_names[index]) == 0)
				break;
			}
			
			map_create_remove_tile(index, map_create_ctx);
		    }
		    else
		    {
			map_create_add_tile(btn->text, map_create_ctx);
		    }
		}

		draw_button(pixel_map, btn, false);

		y += 1;
		if (y >= map_create_ctx->tile_page_height)
		{
		    y = 0;
		    x += 1;
		}

	    }



	    break;
	}

	case SETTINGS_VIEW:
	{
	    bool in_button; // reused variable

	    // Updating increment values
	    update_increment_value(&map_create_ctx->settings_width, generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);
	    update_increment_value(&map_create_ctx->settings_height, generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);
	    update_increment_value(&map_create_ctx->settings_tile_width, generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);
	    update_increment_value(&map_create_ctx->settings_tile_height, generic_ctx->mouse_pos, generic_ctx->mouse_clicked, generic_ctx->mouse_released);

	    // Drawing
	    in_button = is_inside_collision_box(&map_create_ctx->settings_save_btn.col_box, generic_ctx->mouse_pos);
	    draw_button(pixel_map, &map_create_ctx->settings_save_btn, in_button);

	    draw_increment_value(pixel_map, &map_create_ctx->settings_width, generic_ctx->mouse_pos);
	    draw_increment_value(pixel_map, &map_create_ctx->settings_height, generic_ctx->mouse_pos);
	    draw_increment_value(pixel_map, &map_create_ctx->settings_tile_width, generic_ctx->mouse_pos);
	    draw_increment_value(pixel_map, &map_create_ctx->settings_tile_height, generic_ctx->mouse_pos);

	    break;
	}

    }

    return return_code;
}

void map_create_end(MapCreateCtx *map_create_ctx)
{
    // Tile Manager
    for (int i = 0; i < map_create_ctx->available_tiles_amt; ++i)
    {
	free((char*)map_create_ctx->available_tiles[i].text);
    }
    free(map_create_ctx->available_tiles);

    // Collision map
    glDeleteTextures(1, &map_create_ctx->cross_tex);
    glDeleteTextures(1, &map_create_ctx->check_tex);

    // Board Map
    map_create_ctx->tile_quad.~Quad();
    map_create_ctx->tile_shader.~Shader();

    free(map_create_ctx->tile_buttons);

    glDeleteTextures(1, &map_create_ctx->bg_tex);
    map_create_ctx->bg_quad.~Quad();
    map_create_ctx->bg_shader.~Shader();

    // General
    text_box_destroy(&map_create_ctx->selector);
    snake_map_destroy(map_create_ctx->map);
    free(map_create_ctx);
}

void map_create_add_tile(const char *name, MapCreateCtx *map_create_ctx)
{
    ++map_create_ctx->map->tiles_amt;
    map_create_ctx->map->tile_names = (char**)reallocarray(map_create_ctx->map->tile_names, map_create_ctx->map->tiles_amt, sizeof(char*));
    map_create_ctx->map->tile_textures = (GLuint*)reallocarray(map_create_ctx->map->tile_textures, map_create_ctx->map->tiles_amt, sizeof(GLuint));
    map_create_ctx->map->tile_maps = (PixelMap*)reallocarray(map_create_ctx->map->tile_maps, map_create_ctx->map->tiles_amt, sizeof(PixelMap));

    int index = map_create_ctx->map->tiles_amt - 1;

    // copy name
    map_create_ctx->map->tile_names[index] = (char*)calloc(256, sizeof(char));
    strcpy(map_create_ctx->map->tile_names[index], name);

    // Copy texture
    char buf[256];
    snprintf(buf, 256, "%s%s%s", g_tile_dir, name, g_tile_file_extension);

    map_create_ctx->map->tile_maps[index] = pixel_map_create(map_create_ctx->map->tile_width, map_create_ctx->map->tile_height);
    load_tile_from_file(buf, &map_create_ctx->map->tile_maps[index]);

    glGenTextures(1, &map_create_ctx->map->tile_textures[index]);
    glBindTexture(GL_TEXTURE_2D, map_create_ctx->map->tile_textures[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map_create_ctx->map->tile_maps[index].width, map_create_ctx->map->tile_maps[index].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, map_create_ctx->map->tile_maps[index].data);

    // set all to red
    for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
    {
	memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{255, 0, 0}, sizeof(Vec3i));
    }
    
    // search for green
    for (int i = 0; i < map_create_ctx->map->tiles_amt; ++i)
    {
	const char *tile_name = map_create_ctx->map->tile_names[i];
	
	for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
	{
	    if (strcmp(tile_name, map_create_ctx->available_tiles[tile_num].text) == 0)
	    {
		memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{0, 150, 0}, sizeof(Vec3i));
		break;
	    }
	}
    }

    // recreate the tile lines in board map
    free(map_create_ctx->tile_buttons);

    int tile_width = map_create_ctx->tiles_dimensions[0];
    int tile_height = map_create_ctx->tiles_dimensions[1];
    Vec2i pos;
    pos[0] = map_create_ctx->tiles_pos[0];
    pos[1] = map_create_ctx->tiles_pos[1];
    map_create_ctx->tile_buttons_amt = map_create_ctx->map->tiles_amt + 1;
    map_create_ctx->tile_buttons = (TileButton*)calloc(map_create_ctx->tile_buttons_amt, sizeof(TileButton));
    map_create_ctx->tile_buttons[0] = tile_button_create(-1, tile_width, tile_height, pos);
    for (int i = 1; i < map_create_ctx->tile_buttons_amt; ++i)
    {
	map_create_ctx->tile_buttons[i] = tile_button_create(i - 1, tile_width, tile_height, Vec2i{pos[0] , pos[1] - i * tile_height + (i / map_create_ctx->tiles_line_length * map_create_ctx->tiles_line_length * map_create_ctx->tiles_dimensions[1])});
    }
}

void map_create_remove_tile(int index, MapCreateCtx *map_create_ctx)
{
    // Remove tile from boardmap
    for (int i = 0; i < map_create_ctx->map->size; ++i)
    {
	if (map_create_ctx->map->board_map[i] > index)
	{
	    map_create_ctx->map->board_map[i] -= 1;
	    continue;
	}

	if (map_create_ctx->map->board_map[i] == index)
	{
	    map_create_ctx->map->board_map[i] = -1;
	}
    }

    if (map_create_ctx->selected_tile == index)
    {
	map_create_ctx->selected_tile = -1;
    }

    // Delete the resources
    pixel_map_destroy(&map_create_ctx->map->tile_maps[index]);
    glDeleteTextures(1, &map_create_ctx->map->tile_textures[index]);
    free(map_create_ctx->map->tile_names[index]);

    // move the resources to fill space
    for (int i = index; i < map_create_ctx->map->tiles_amt - 1; ++i)
    {
	map_create_ctx->map->tile_maps[i] = map_create_ctx->map->tile_maps[i + 1];
	map_create_ctx->map->tile_names[i] = map_create_ctx->map->tile_names[i + 1];
	map_create_ctx->map->tile_textures[i] = map_create_ctx->map->tile_textures[i + 1];
    }

    // Reallocate the arrays
    --map_create_ctx->map->tiles_amt;
    map_create_ctx->map->tile_names = (char**)reallocarray(map_create_ctx->map->tile_names, map_create_ctx->map->tiles_amt, sizeof(char*));
    map_create_ctx->map->tile_textures = (GLuint*)reallocarray(map_create_ctx->map->tile_textures, map_create_ctx->map->tiles_amt, sizeof(GLuint));
    map_create_ctx->map->tile_maps = (PixelMap*)reallocarray(map_create_ctx->map->tile_maps, map_create_ctx->map->tiles_amt, sizeof(PixelMap));    

    // set all to red
    for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
    {
	memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{255, 0, 0}, sizeof(Vec3i));
    }
    
    // search for green
    for (int i = 0; i < map_create_ctx->map->tiles_amt; ++i)
    {
	const char *tile_name = map_create_ctx->map->tile_names[i];
	
	for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
	{
	    if (strcmp(tile_name, map_create_ctx->available_tiles[tile_num].text) == 0)
	    {
		memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{0, 150, 0}, sizeof(Vec3i));
		break;
	    }
	}
    }

    // recreate the tile lines in board map
    free(map_create_ctx->tile_buttons);
    int tile_width = map_create_ctx->tiles_dimensions[0];
    int tile_height = map_create_ctx->tiles_dimensions[1];
    Vec2i pos;
    pos[0] = map_create_ctx->tiles_pos[0];
    pos[1] = map_create_ctx->tiles_pos[1];
    map_create_ctx->tile_buttons_amt = map_create_ctx->map->tiles_amt + 1;
    map_create_ctx->tile_buttons = (TileButton*)calloc(map_create_ctx->tile_buttons_amt, sizeof(TileButton));
    map_create_ctx->tile_buttons[0] = tile_button_create(-1, tile_width, tile_height, pos);
    for (int i = 1; i < map_create_ctx->tile_buttons_amt; ++i)
    {
	map_create_ctx->tile_buttons[i] = tile_button_create(i - 1, tile_width, tile_height, Vec2i{pos[0] , pos[1] - i * tile_height + (i / map_create_ctx->tiles_line_length * map_create_ctx->tiles_line_length * map_create_ctx->tiles_dimensions[1])});
    }
}


void map_create_load_map(MapCreateCtx *map_create_ctx)
{

    char buf[256];
    snprintf(buf, 256, "%s%s%s", g_map_dir, map_create_ctx->selector.text, g_map_file_extension);
    
    SnakeMap *result = snake_map_create(buf);
    if (result == NULL)
	return;

    // Board Map stuff
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
	
	free(map_create_ctx->tile_buttons);
	float tile_aspect_ratio = (float)map_create_ctx->map->tile_width / map_create_ctx->map->tile_height;
	if (tile_aspect_ratio > 1)
	{
	    float width = 50;
	    float height = width / tile_aspect_ratio;
	    
	    map_create_ctx->tiles_dimensions[0] = width;
	    map_create_ctx->tiles_dimensions[1] = height;
	}
	else
	{
	    float height = 50;
	    float width = height * tile_aspect_ratio;
	    
	    map_create_ctx->tiles_dimensions[0] = width;
	    map_create_ctx->tiles_dimensions[1] = height;
	}
	map_create_ctx->tiles_line_length = (map_create_ctx->tiles_pos[1]) / map_create_ctx->tiles_dimensions[1];
	map_create_ctx->current_tiles_line = 0;
	
	int tile_width = map_create_ctx->tiles_dimensions[0];
	int tile_height = map_create_ctx->tiles_dimensions[1];
	Vec2i pos;
	pos[0] = map_create_ctx->tiles_pos[0];
	pos[1] = map_create_ctx->tiles_pos[1];
	map_create_ctx->tile_buttons_amt = map_create_ctx->map->tiles_amt + 1;
	map_create_ctx->tile_buttons = (TileButton*)calloc(map_create_ctx->tile_buttons_amt, sizeof(TileButton));
	map_create_ctx->tile_buttons[0] = tile_button_create(-1, tile_width, tile_height, pos);
	for (int i = 1; i < map_create_ctx->tile_buttons_amt; ++i)
	{
	    map_create_ctx->tile_buttons[i] = tile_button_create(i - 1, tile_width, tile_height, Vec2i{pos[0] , pos[1] - i * tile_height + (i / map_create_ctx->tiles_line_length * map_create_ctx->tiles_line_length * map_create_ctx->tiles_dimensions[1])});
	}
	
    }


    // Tile Manager stuff
    {
	for (int i = 0; i < map_create_ctx->available_tiles_amt; ++i)
	{
	    free((char*)map_create_ctx->available_tiles[i].text);
	}
	free(map_create_ctx->available_tiles);

	// Get amount of tile files
	std::string path = g_tile_dir;
	map_create_ctx->available_tiles_amt = 0;
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
	    if (entry.is_regular_file())
	    {
		std::string path_name_str = entry.path().string();
		const char* path_name = path_name_str.c_str();
		if (strlen(path_name) >= strlen(g_tile_file_extension) && memcmp(path_name + (strlen(path_name) - strlen(g_tile_file_extension)), g_tile_file_extension, strlen(g_tile_file_extension)) == 0)
		{
		    int height;
		    int width;
		    get_dimensions_of_tile_file(path_name, &width, &height);
		    if (height == map_create_ctx->map->tile_height && width == map_create_ctx->map->tile_width)
			++map_create_ctx->available_tiles_amt;
		}
	    }
	}
	
	// Create the buttons for the tiles
	int num = 0;
	map_create_ctx->available_tiles = (Button*)calloc(map_create_ctx->available_tiles_amt, sizeof(Button));
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
	    if (entry.is_regular_file())
	    {
		std::string path_name_str = entry.path().string();
		const char *path_name = path_name_str.c_str();
		if (strlen(path_name) >= strlen(g_tile_file_extension) && memcmp(path_name + (strlen(path_name) - strlen(g_tile_file_extension)), g_tile_file_extension, strlen(g_tile_file_extension)) == 0)
		{
		    
		    int height;
		    int width;
		    get_dimensions_of_tile_file(path_name, &width, &height);
		    if (height == map_create_ctx->map->tile_height && width == map_create_ctx->map->tile_width)
		    {
			const char *path = strrchr(path_name, '/') + 1;
			const int path_size = strlen(path) - 5;
			
			char *new_path = (char*)calloc(path_size + 1, sizeof(char));
			memcpy(new_path, path, path_size);
			new_path[path_size] = '\0';
			
			map_create_ctx->available_tiles[num] = create_button((const char*)new_path, 200, 30, Vec2i{0, 0}, Vec3i{50, 50, 50}, Vec3i{255, 255, 255}, 8, 1);
			++num;
		    }
		}
	    }
	}
	
	// highlight in use tiles
	// set all to red
	for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
	{
	    memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{255, 0, 0}, sizeof(Vec3i));
	}
	
	// search for green
	for (int i = 0; i < map_create_ctx->map->tiles_amt; ++i)
	{
	    const char *tile_name = map_create_ctx->map->tile_names[i];
	    
	    for (int tile_num = 0; tile_num < map_create_ctx->available_tiles_amt; ++tile_num)
	    {
		if (strcmp(tile_name, map_create_ctx->available_tiles[tile_num].text) == 0)
		{
		    memcpy(map_create_ctx->available_tiles[tile_num].bg_color, Vec3i{0, 150, 0}, sizeof(Vec3i));
		    break;
		}
	    }
	    
	}

    }
    
}

TileButton tile_button_create(int tile, int width, int height, Vec2i pos)
{
    TileButton result;
    result.tile = tile;
    result.col_box.bottom_left[0] = pos[0];
    result.col_box.bottom_left[1] = pos[1];
    result.col_box.top_right[0] = pos[0] + width;
    result.col_box.top_right[1] = pos[1] + height;
    return result;
}

void draw_tile_button(TileButton *tile_button, GLuint tex, Quad *quad, Shader *shader, glm::mat4 vp, GLuint bg)
{
    shader->use();
    glUniform1i(glGetUniformLocation(shader->m_program, "u_tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shader->m_program, "u_vp"), 1, GL_FALSE, glm::value_ptr(vp));

    quad->m_position[0] = tile_button->col_box.bottom_left[0];
    quad->m_position[1] = tile_button->col_box.bottom_left[1];

    quad->m_scale[0] = tile_button->col_box.top_right[0] - tile_button->col_box.bottom_left[0];
    quad->m_scale[1] = tile_button->col_box.top_right[1] - tile_button->col_box.bottom_left[1];
    quad->update_model();

    Quad bg_quad = *quad;
    bg_quad.m_position[2] = quad->m_position[2] - 0.1;
    bg_quad.update_model();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bg);

    bg_quad.update_model();
    glUniformMatrix4fv(glGetUniformLocation(shader->m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(bg_quad.m_model_matrix));

    bg_quad.draw();

    glBindTexture(GL_TEXTURE_2D, tex);
    glUniformMatrix4fv(glGetUniformLocation(shader->m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(quad->m_model_matrix));
    quad->draw();
}


