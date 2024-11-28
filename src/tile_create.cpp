#include "tile_create.hpp"
#include "collisions.hpp"
#include "common.hpp"
#include "drawing.hpp"
#include "game_state.hpp"
#include "pixel_map.hpp"
#include "map.hpp"
#include "quad.hpp"
#include "text_box.hpp"

#include <cstdlib>
#include <cstring>

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

TileCreateCtx* tile_create_start(GenericCtx *generic_ctx)
{
    TileCreateCtx *ctx = (TileCreateCtx*)calloc(1, sizeof(TileCreateCtx));
    ctx->tile_width = generic_ctx->settings.tile_create_width;
    ctx->tile_height = generic_ctx->settings.tile_create_height;
    ctx->tile_size = ctx->tile_width * ctx->tile_height;
    ctx->tile = (RGBAPixel*)calloc(ctx->tile_size, sizeof(RGBAPixel));

    // Background
    new (&ctx->bg_quad) Quad;
    new (&ctx->bg_shader) Shader(g_shader_bg_vertex_code, g_shader_bg_fragment_code);
    
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



    for (int i = 0 ; i < ctx->tile_size; ++i)
    {
	ctx->tile[i].r = 0;
	ctx->tile[i].g = 0;
	ctx->tile[i].b = 0;
	ctx->tile[i].a = 0;
    }
    ctx->tile_pixmap = pixel_map_create(400, 400);

    ctx->red_slider = slider_create(0, 255, 600, 30, Vec2i{100, 90});
    ctx->red_slider.value = 255;
    ctx->green_slider = slider_create(0, 255, 600, 30, Vec2i{100, 50});
    ctx->green_slider.value = 255;
    ctx->blue_slider = slider_create(0, 255, 600, 30, Vec2i{100, 10});
    ctx->blue_slider.value = 255;
    ctx->alpha_slider = slider_create(0, 255, 600, 30, Vec2i{100, 650});
    ctx->alpha_slider.value = 255;

    ctx->current_color.r = ctx->red_slider.value;
    ctx->current_color.g = ctx->green_slider.value;
    ctx->current_color.b = ctx->blue_slider.value;
    ctx->current_color.a = ctx->alpha_slider.value;

    for (int i = 0; i < ctx->old_colors_amt; ++i)
    {
	ctx->old_colors[i].r = 0;
	ctx->old_colors[i].g = 0;
	ctx->old_colors[i].b = 0;
	ctx->old_colors[i].a = 0;
    }

    ctx->text = text_box_create(25, Vec2i{150, 700}, 8, 2);
    strcpy(ctx->text.text, "new");

    ctx->save_btn = create_button("save", 100, 32, Vec2i{590, 700}, Vec3i{50, 50, 50}, Vec3i{255, 255, 0}, 8, 2)
;
    memcpy(&ctx->save_btn.bg_hover_color, &ctx->save_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->save_btn.fg_hover_color, &ctx->save_btn.bg_color, sizeof(Vec3i));

    ctx->load_btn = create_button("load", 100, 32, Vec2i{690, 700}, Vec3i{50, 50, 50}, Vec3i{0, 200, 0}, 8, 2);
    memcpy(&ctx->load_btn.bg_hover_color, &ctx->load_btn.fg_color, sizeof(Vec3i));
    memcpy(&ctx->load_btn.fg_hover_color, &ctx->load_btn.bg_color, sizeof(Vec3i));

    return ctx;
}

GameReturnCode tile_create_run(PixelMap *pixel_map, GenericCtx *generic_ctx, TileCreateCtx *tile_create_ctx)
{
    GameReturnCode return_code = GameReturnCode::none;

    if (generic_ctx->keyboard.escape)
    {
	return_code = GameReturnCode::goto_menu;
    }



    Vec2i tile_pos = {(pixel_map->width - tile_create_ctx->tile_pixmap.width) / 2, (pixel_map->height - tile_create_ctx->tile_pixmap.height) / 2};
    int pixel_width = tile_create_ctx->tile_pixmap.width / tile_create_ctx->tile_width;
    int pixel_height = tile_create_ctx->tile_pixmap.height / tile_create_ctx->tile_height;

    // Draw background

    tile_create_ctx->bg_quad.m_position[0] = tile_pos[0];
    tile_create_ctx->bg_quad.m_position[1] = tile_pos[1];
    tile_create_ctx->bg_quad.m_position[2] = -11.f;
    tile_create_ctx->bg_quad.m_scale[0] = tile_create_ctx->tile_pixmap.width;
    tile_create_ctx->bg_quad.m_scale[1] = tile_create_ctx->tile_pixmap.height;

    glBindTexture(GL_TEXTURE_2D, tile_create_ctx->bg_tex);
    tile_create_ctx->bg_quad.update_model();
    tile_create_ctx->bg_shader.use();
    glUniform1i(glGetUniformLocation(tile_create_ctx->bg_shader.m_program, "u_tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(tile_create_ctx-> bg_shader.m_program, "u_vp"), 1, GL_FALSE, glm::value_ptr(generic_ctx->vp_matrix));
    glUniformMatrix4fv(glGetUniformLocation(tile_create_ctx->bg_shader.m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(tile_create_ctx->bg_quad.m_model_matrix));
    glUniform1i(glGetUniformLocation(tile_create_ctx->bg_shader.m_program, "u_tile_x_amt"), tile_create_ctx->tile_width);
    glUniform1i(glGetUniformLocation(tile_create_ctx->bg_shader.m_program, "u_tile_y_amt"), tile_create_ctx->tile_height);
    tile_create_ctx->bg_quad.draw();


    Vec2i old_colors_pos = {0, 300};
    const int old_color_pixel_width = 30;
    const int old_color_pixel_height = 30;

    // see if old colors is clicked
    {
	if (generic_ctx->mouse_clicked && generic_ctx->mouse_pos[0] >= old_colors_pos[0] && generic_ctx->mouse_pos[0] < old_colors_pos[0] + old_color_pixel_width && generic_ctx->mouse_pos[1] >= old_colors_pos[1] && generic_ctx->mouse_pos[1] < old_colors_pos[1] + tile_create_ctx->old_colors_amt * old_color_pixel_height)
	{
	    int y = (generic_ctx->mouse_pos[1] - old_colors_pos[1]) / old_color_pixel_height;	    

	    tile_create_ctx->red_slider.value = tile_create_ctx->old_colors[y].r;
	    tile_create_ctx->green_slider.value = tile_create_ctx->old_colors[y].g;
	    tile_create_ctx->blue_slider.value = tile_create_ctx->old_colors[y].b;
	    tile_create_ctx->alpha_slider.value = tile_create_ctx->old_colors[y].a;
	}
    }

    // See if right clicked tile
    if (generic_ctx->mouse_right_clicked)
    {
	int x = (generic_ctx->mouse_pos[0] - tile_pos[0]) / pixel_width;
	int y = (generic_ctx->mouse_pos[1] - tile_pos[1]) / pixel_height;

	if (x >= 0 && x < tile_create_ctx->tile_width &&
	    y >= 0 && y < tile_create_ctx->tile_height)
	{
	    const int index = x + y * tile_create_ctx->tile_width;
	    tile_create_ctx->red_slider.value = tile_create_ctx->tile[index].r;
	    tile_create_ctx->green_slider.value = tile_create_ctx->tile[index].g;
	    tile_create_ctx->blue_slider.value = tile_create_ctx->tile[index].b;
	    tile_create_ctx->alpha_slider.value = tile_create_ctx->tile[index].a;
	}
    }

    // update sliders
    slider_update(&tile_create_ctx->red_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->green_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->blue_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);
    slider_update(&tile_create_ctx->alpha_slider, generic_ctx->mouse_pos, generic_ctx->mouse_clicked);

    // update current color
    tile_create_ctx->current_color.r = tile_create_ctx->red_slider.value;
    tile_create_ctx->current_color.g = tile_create_ctx->green_slider.value;
    tile_create_ctx->current_color.b = tile_create_ctx->blue_slider.value;
    tile_create_ctx->current_color.a = tile_create_ctx->alpha_slider.value;

    // Updating tile
    if (generic_ctx->mouse_clicked)
    {
	int x = (generic_ctx->mouse_pos[0] - tile_pos[0]) / pixel_width;
	int y = (generic_ctx->mouse_pos[1] - tile_pos[1]) / pixel_height;

	if (x >= 0 && x < tile_create_ctx->tile_width &&
	    y >= 0 && y < tile_create_ctx->tile_height)
	{
	    tile_create_ctx->tile[x + y * tile_create_ctx->tile_width] = tile_create_ctx->current_color;

	    // See if color is already in old colors
	    int index_of_old_color = tile_create_ctx->old_colors_amt - 1;
	    for (int i = 0; i < tile_create_ctx->old_colors_amt; ++i)
	    {
		RGBAPixel cur = tile_create_ctx->current_color;
		RGBAPixel now = tile_create_ctx->old_colors[i];

		if (cur.r == now.r && cur.g == now.g && cur.b == now.b && cur.a == now.a)
		{
		    index_of_old_color = i;
		    break;
		}
	    }

	    // Actually put color into old colors
	    for (int i = index_of_old_color; i > 0; --i)
	    {
		tile_create_ctx->old_colors[i] = tile_create_ctx->old_colors[i - 1];
	    }
	    tile_create_ctx->old_colors[0] = tile_create_ctx->current_color;
	}

    }


    // Drawing old colors
    {
	Vec3i bg = {50, 50, 50};
	const int border_size = 3;
	for (int i = 0; i < tile_create_ctx->old_colors_amt; ++i)
	{
	    Vec3i col;
	    col[0] = tile_create_ctx->old_colors[i].r;
	    col[1] = tile_create_ctx->old_colors[i].g;
	    col[2] = tile_create_ctx->old_colors[i].b;

	    Vec2i border_pos;
	    border_pos[0] = old_colors_pos[0];
	    border_pos[1] = old_colors_pos[1] + i * old_color_pixel_height;

	    Vec2i pos;
	    pos[0] = border_pos[0] + border_size;
	    pos[1] = border_pos[1] + border_size;

	    draw_rectangle(pixel_map, 30, 30, border_pos, bg, 255);
	    draw_rectangle(pixel_map, old_color_pixel_width - border_size * 2, old_color_pixel_height - border_size * 2, pos, col, tile_create_ctx->old_colors[i].a);
	}
    }

    // Drawing current color
    {
	Vec3i color;
	color[0] = tile_create_ctx->current_color.r;
	color[1] = tile_create_ctx->current_color.g;
	color[2] = tile_create_ctx->current_color.b;
	draw_rectangle(pixel_map, pixel_map->width, 30, Vec2i{0, 130}, color, 255);

	char buf[256];
	snprintf(buf, 256, "# %02x%02x%02x %02x", color[0], color[1], color[2], tile_create_ctx->current_color.a);

	draw_sentence(pixel_map, buf, 8, 2, Vec2i{0, 170}, Vec3i{255, 255, 255});

    }

    // draw sliders
    draw_slider(pixel_map, &tile_create_ctx->red_slider, Vec3i{255, 0, 0});
    draw_slider(pixel_map, &tile_create_ctx->green_slider, Vec3i{0, 255, 0});
    draw_slider(pixel_map, &tile_create_ctx->blue_slider, Vec3i{0, 0, 255});
    draw_slider(pixel_map, &tile_create_ctx->alpha_slider, Vec3i{255, 255, 255});

    // Drawing to pixmap
    for (int y = 0; y < tile_create_ctx->tile_height; ++y)
    {
	int y_offset = y * tile_create_ctx->tile_width;
	for (int x = 0; x < tile_create_ctx->tile_width; ++x)
	{
	    int index = y_offset + x;
	    Vec3i color;
	    color[0] = tile_create_ctx->tile[index].r;
	    color[1] = tile_create_ctx->tile[index].g;
	    color[2] = tile_create_ctx->tile[index].b;

	    draw_rectangle(&tile_create_ctx->tile_pixmap, pixel_width, pixel_height, Vec2i{x * pixel_width, y *pixel_height}, color, tile_create_ctx->tile[index].a);
	}
    }    

    // Drawing tile
    draw_pixmap(pixel_map, &tile_create_ctx->tile_pixmap, tile_pos);

    // Drawing text box
    text_box_parse_key_list(&tile_create_ctx->text, generic_ctx->key_list);
    draw_text_box(pixel_map, &tile_create_ctx->text, true);
    draw_sentence(pixel_map, "name", 8, 2, Vec2i{80, 708}, Vec3i{255, 255, 0});

    // save btn
    {
	bool hover = is_inside_collision_box(&tile_create_ctx->save_btn.col_box, generic_ctx->mouse_pos);

	draw_button(pixel_map, &tile_create_ctx->save_btn, hover);

	if (generic_ctx->mouse_released && hover)
	{
	    char buf[255] = {0};
	    strcpy(buf, g_tile_dir);
	    strcat(buf, tile_create_ctx->text.text);
	    strcat(buf, g_tile_file_extension);

	    write_tile_to_file(buf, tile_create_ctx->tile, tile_create_ctx->tile_width, tile_create_ctx->tile_height);
	}
    }

    // load btn
    {
	bool hover = is_inside_collision_box(&tile_create_ctx->load_btn.col_box, generic_ctx->mouse_pos);

	draw_button(pixel_map, &tile_create_ctx->load_btn, hover);

	if (generic_ctx->mouse_released && hover)
	{
	    char buf[255] = {0};
	    strcpy(buf, g_tile_dir);
	    strcat(buf, tile_create_ctx->text.text);
	    strcat(buf, g_tile_file_extension);

	    PixelMap new_map = pixel_map_create(tile_create_ctx->tile_width, tile_create_ctx->tile_height);

	    int status = load_tile_from_file(buf, &new_map);

	    if (status == 0)
	    {
		for (int y = 0; y < new_map.height; ++y)
		{
		    for (int x = 0; x < new_map.height; ++x)
		    {
			const int index = y * new_map.width + x;
			tile_create_ctx->tile[index] = new_map.data[index];
		    }
		}
		
		pixel_map_destroy(&new_map);
	    }
	}
    }

    return return_code;
}

void tile_create_end(TileCreateCtx *tile_create_ctx)
{
    tile_create_ctx->bg_quad.~Quad();
    tile_create_ctx->bg_shader.~Shader();
    glDeleteTextures(1, &tile_create_ctx->bg_tex);

    free(tile_create_ctx->tile);
    text_box_destroy(&tile_create_ctx->text);
    pixel_map_destroy(&tile_create_ctx->tile_pixmap);
    free(tile_create_ctx);
}
