#include <cstdio>
#include <cstring>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.hpp"
#include "drawing.hpp"
#include "shader.hpp"
#include "quad.hpp"
#include "pixel.hpp"
#include "pixel_map.hpp"
#include "custom_fonts.hpp"
#include "game_state.hpp"
#include "button.hpp"
#include "context.hpp"
#include "menu.hpp"
#include "snake.hpp"
#include "settings.hpp"
#include "tile_create.hpp"
#include "map_create.hpp"

double scroll_y_offset = 0.0;
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

char key_list[256] = {0}; // This will hold a list of keys that were pressed in chronological order
char key_last_pressed = '\0';
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Pixel Map Settings
    PixelMap pixel_map = pixel_map_create(pix_width, pix_height);
    Shader shader(g_shader_vertex_code, g_shader_fragment_code);
    Quad quad;
    quad.scale(pix_width, pix_height);
    quad.translate(0, 0, -10.0f);
    quad.update_model();
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pix_width, pix_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_map.data);

    // Fonts
    init_fonts();

    // Initialize generic_context
    GenericCtx generic_context;
    generic_context.pix_width = pix_width;
    generic_context.pix_height = pix_height;
    generic_context.game_state = GameState::menu;
    generic_context.settings = settings_from_file(g_settings_file);
    generic_context.key_list = key_list;
    generic_context.mouse_clicked = false;
    generic_context.mouse_released = false;
    generic_context.mouse_right_clicked = false;
    
    // Create vp matrix. (0, 0) is bottom left corner, and (800, 800) is top right corner
    glm::mat4 proj = glm::ortho(0.0f, 800.f, 0.0f, 800.f, 0.1f, 100.f);
    generic_context.vp_matrix = proj;


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

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	memset(pixel_map.data, 0, pixel_map.size * sizeof(RGBAPixel));
	for (int i = 0; i < pixel_map.size; ++i)
	{
	    pixel_map.data[i].a = 255;
	}
	
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
	    generic_context.mouse_right_clicked = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

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
	generic_context.keyboard.escape = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
	generic_context.mouse_scroll = scroll_y_offset;
	generic_context.last_pressed_key = key_last_pressed;	    

	
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
		return_code = map_create_run(&pixel_map, &generic_context, (MapCreateCtx*)specific_context);
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
		    MapCreateCtx *map_create_ctx = (MapCreateCtx*)specific_context;
		    map_create_end(map_create_ctx);		    
		    break;
		}
		case GameState::tile_create:
		{
		    TileCreateCtx *tile_create_ctx = (TileCreateCtx*)specific_context;
		    tile_create_end(tile_create_ctx);		    
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
	    case GameReturnCode::goto_map_create:
	    {
		specific_context = (void*)map_create_start(&generic_context);
		generic_context.game_state = GameState::map_create;
		break;		
	    }

	}

#ifndef NDEBUG
	// Draw Top Bar
	draw_rectangle(&pixel_map, pix_width, 50, Vec2i{0, 750}, Vec3i{50, 50, 50}, 255);
	draw_sentence(&pixel_map, "snake maker", 8, 3, Vec2i{30, 760}, Vec3i{0, 255, 0});
	char buf[255];
	memset(buf, 0, 255);
	snprintf(buf, 255, "fps %.2lf", 1.0 / generic_context.delta_time);
	draw_sentence(&pixel_map, buf, 8, 3, Vec2i{550, 760}, Vec3i{255, 0, 0});
#endif

	// Drawing the pixmap onto the screen
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pix_width, pix_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_map.data);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.m_program, "u_tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.m_program, "u_model"), 1, GL_FALSE, glm::value_ptr(quad.m_model_matrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.m_program, "u_vp"), 1, GL_FALSE, glm::value_ptr(generic_context.vp_matrix));
	quad.draw();

	// Reset variables that are set by input callbacks
	scroll_y_offset = 0.0;
	memset(key_list, 0, 256);
	key_last_pressed = '\0';

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    
    // Stop the current state
    switch (generic_context.game_state)
    {
	case GameState::menu:
	{
	    MenuCtx *menu_ctx = (MenuCtx*)specific_context;
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
	    MapCreateCtx *map_create_ctx = (MapCreateCtx*)specific_context;
	    map_create_end(map_create_ctx);		    
	    break;
	}
	case GameState::tile_create:
	{
	    TileCreateCtx *tile_create_ctx = (TileCreateCtx*)specific_context;
	    tile_create_end(tile_create_ctx);		    
	    break;
	}
    }
    
    pixel_map_destroy(&pixel_map);

    glDeleteTextures(1, &tex);

    glfwDestroyWindow(window);
//    glfwTerminate(); this is proper, however glfw has a bug which makes this cause a segfault, so I simply don't use this

    return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scroll_y_offset = yoffset;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // For last pressed key
    if (action == GLFW_PRESS)
    {
	key_last_pressed = key;
    }
    
    // For typing input (key_list global variable)
    if (action != GLFW_RELEASE)
    {
	
	char c[2] = {0};
	
	switch (key)
	{
	    case GLFW_KEY_SPACE:
		c[0] = ' ';
		break;
	    case GLFW_KEY_BACKSPACE:
		c[0] = '\b';
		break;
	    case GLFW_KEY_MINUS:
		if (mods && GLFW_MOD_SHIFT)
		    c[0] = '_';
		else
		    c[0] = '-';
		break;
	    case GLFW_KEY_PERIOD:
		c[0] = '.';
		break;
	}
	
	
	if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
	{
	    if (mods && GLFW_MOD_SHIFT)
	    {
		c[0] = key;
	    }
	    else
	    {
		c[0] = key + 32;
	    }
	}
	
	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
	    if (!(mods && GLFW_MOD_SHIFT))
		c[0] = key;
	}
	
	strcat(key_list, c);
    }
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    win_width = width;
    win_height = height;
}
