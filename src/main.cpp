#include <cstdio>
#include <cstring>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "common.hpp"
#include "drawing.hpp"
#include "shader.hpp"
#include "quad.hpp"
#include "pixel.hpp"
#include "pixel_map.hpp"
#include "custom_fonts.hpp"
#include "game_state.hpp"
#include "collisions.hpp"
#include "button.hpp"
#include "context.hpp"
#include "menu.hpp"
#include "snake.hpp"
#include "settings.hpp"
#include "tile_create.hpp"

double scroll_y_offset = 0.0;
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

char key_list[256] = {0}; // This will hold a list of keys that were pressed in chronological order
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void window_resize_callback(GLFWwindow *window, int width, int height);

const int pix_width = 800;
const int pix_height = 800;

int win_width = 800;
int win_height = 800;

int main(void)
{
    // GLFW Configuration
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "Snake Maker", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(true); // turns on vsync
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowAspectRatio(window, 1, 1);
    glfwSetWindowSizeCallback(window, window_resize_callback);

    // Load the opengl functions
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return 1;
    }

    // Viewport settings
    glViewport(0, 0, win_width, win_height);

    // Pixel Map Settings
    PixelMap pixel_map = pixel_map_create(pix_width, pix_height);
    Shader shader(g_shader_vertex_code, g_shader_fragment_code);
    Quad quad;
    float border_color[] = {0.0, 0.0, 0.0, 0.0};
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pix_width, pix_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_map.data);

    // Fonts
    init_fonts();

    // Initialize generic_context
    GenericCtx generic_context;
    generic_context.game_state = GameState::menu;
    generic_context.settings = settings_from_file(g_settings_file);

    // Initialize specific_context
    void* specific_context = (void*)menu_start(&generic_context);

    // Loop variables
    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_end = std::chrono::high_resolution_clock::now();
   
    // Render Loop
    while(!glfwWindowShouldClose(window))
    {

	// Calculate delta time
	time_end = std::chrono::high_resolution_clock::now();
	generic_context.delta_time = std::chrono::duration<double>(time_end-time_start).count();
	time_start = time_end;
	
	
	// Fill generic context
	{
	    double xpos, ypos;
	    glfwGetCursorPos(window, &xpos, &ypos);

	    xpos = (double)xpos / win_width * pix_width;
	    ypos = (1.0 - (double)ypos / win_height) * pix_height;

	    generic_context.mouse_pos[0] = (int)xpos;
	    generic_context.mouse_pos[1] = (int)ypos;
	}	
	{
	    bool old_clicked = generic_context.mouse_clicked;
	    generic_context.mouse_clicked = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	    if (old_clicked == true && generic_context.mouse_clicked == false)
	    {
		generic_context.mouse_released = true;
	    }
	    else
	    {
		generic_context.mouse_released = false;
	    }
	}

	generic_context.keyboard.w = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	generic_context.keyboard.a = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	generic_context.keyboard.s = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	generic_context.keyboard.d = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	generic_context.keyboard.space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	generic_context.keyboard.enter = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
	generic_context.keyboard.backspace = (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
	generic_context.mouse_scroll = scroll_y_offset;
	if (strlen(key_list) != 0)
	    generic_context.last_pressed_key = key_list[strlen(key_list) - 1];	    
	else
	    generic_context.last_pressed_key = '\0';

	
	// Clear
	memset(pixel_map.data, 0, pixel_map.size * sizeof(RGBPixel));

	// Do action for the specified mode/state
	GameReturnCode return_code = GameReturnCode::none;
	switch (generic_context.game_state)
	{
	    case GameState::menu:
	    {
		return_code = menu_run(&pixel_map, &generic_context, (MenuCtx*)specific_context);
		break;
	    }
	    case GameState::snake:
	    {
		return_code = snake_run(&pixel_map, &generic_context, (SnakeCtx*)specific_context);
		break;
	    }
	    case GameState::scoreboard:
	    {
		break;
	    }
	    case GameState::settings:
	    {
		return_code = settings_run(&pixel_map, &generic_context, (SettingsCtx*)specific_context);
		break;
	    }
	    case GameState::map_create:
	    {
		break;
	    }
	    case GameState::tile_create:
	    {
		return_code = tile_create_run(&pixel_map, &generic_context, (TileCreateCtx*)specific_context);
		break;
	    }
	}

	char menu_result[255] = {0};
	// End the current context if specified in return code
	if (return_code != GameReturnCode::none)
	{
	    switch (generic_context.game_state)
	    {
		case GameState::menu:
		{
		    MenuCtx *menu_ctx = (MenuCtx*)specific_context;
		    strcpy(menu_result, menu_ctx->available_maps[menu_ctx->selected_map].text);
		    menu_end(menu_ctx);
		    break;
		}
		case GameState::snake:
		{
		    SnakeCtx *snake_ctx = (SnakeCtx*)specific_context;
		    snake_end(snake_ctx);
		    break;
		}
		case GameState::scoreboard:
		{
		    break;
		}
		case GameState::settings:
		{
		    SettingsCtx *settings_ctx = (SettingsCtx*)specific_context;
		    settings_end(&generic_context, settings_ctx);
		    break;
		}
		case GameState::map_create:
		{
		    break;
		}
		case GameState::tile_create:
		{
		    TileCreateCtx *tile_create_ctx = (TileCreateCtx*)specific_context;
		    tile_create_end(&generic_context, tile_create_ctx);		    
		    break;
		}
	    }
	}


	// Parse the return code
	switch(return_code)
	{
	    case GameReturnCode::none:
	    {
		break;
	    }
	    case GameReturnCode::play_snake:
	    {
		specific_context = (void*)snake_start(&generic_context, menu_result);
		generic_context.game_state = GameState::snake;
		break;
	    }
	    case GameReturnCode::goto_menu:
	    {
		specific_context = (void*)menu_start(&generic_context);
		generic_context.game_state = GameState::menu;		
		break;
	    }
	    case GameReturnCode::goto_settings:
	    {
		specific_context = (void*)settings_start(&generic_context);
		generic_context.game_state = GameState::settings;				
		break;
	    }
	    case GameReturnCode::goto_tile_create:
	    {
		specific_context = (void*)tile_create_start(&generic_context);
		generic_context.game_state = GameState::tile_create;
		break;
	    }
	}

	// Draw Top Bar
	draw_rectangle(&pixel_map, 800, 50, Vec2i{0, 750}, Vec3i{50, 50, 50});
	draw_sentence(&pixel_map, "snake maker", 8, 3, Vec2i{30, 760}, Vec3i{0, 255, 0});
	char buf[255];
	memset(buf, 0, 255);
	snprintf(buf, 255, "fps %.2lf", 1.0 / generic_context.delta_time);
	draw_sentence(&pixel_map, buf, 8, 3, Vec2i{550, 760}, Vec3i{255, 0, 0});

	// Drawing the pixmap onto the screen
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pix_width, pix_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_map.data);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.m_program, "u_tex"), 0);
	quad.draw();

	// Reset variables that are set by input callbacks
	scroll_y_offset = 0.0;
	memset(key_list, 0, 256);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    pixel_map_destroy(&pixel_map);

    return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scroll_y_offset = yoffset;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
    {
	return;
    }

    char c[2] = {0};
    switch (key)
    {
	case GLFW_KEY_W:
	    c[0] = 'w';
	    break;
	case GLFW_KEY_A:
	    c[0] = 'a';
	    break;
	case GLFW_KEY_S:
	    c[0] = 's';
	    break;
	case GLFW_KEY_D:
	    c[0] = 'd';
	    break;

	default:
	    return;
    }

    strcat(key_list, c);
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    win_width = width;
    win_height = height;
}
