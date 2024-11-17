#include <cstdio>
#include <cstring>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "drawing.hpp"
#include "shader.hpp"
#include "quad.hpp"
#include "pixel.hpp"
#include "pixel_map.hpp"
#include "custom_fonts.hpp"
#include "game_state.hpp"
#include "collisions.hpp"
#include "button.hpp"

int main(void)
{
    const int win_width = 800;
    const int win_height = 800;

    // GLFW Configuration
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "Snake Maker", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(true); // turns on vsync
    glfwSetWindowAspectRatio(window, win_width, win_height);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return 1;
    }

    // Viewport settings
    glViewport(0, 0, win_width, win_height);

    // Pixel Map Settings
    PixelMap pixel_map = pixel_map_create(win_width, win_height);
    Shader shader("../shaders/quad.vert", "../shaders/quad.frag");
    Quad quad;
    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    float border_color[] = {0.0, 0.0, 0.0, 0.0};
    glTextureParameterfv(tex, GL_TEXTURE_BORDER_COLOR, border_color);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureStorage2D(tex, 1, GL_RGBA8, pixel_map.width, pixel_map.height);

    // Fonts
    init_fonts();

    GameState game_state = GameState::menu;

    Button button = create_button("test msg", 250, 50, (Vec2i){50, 50}, (Vec3i){255, 255, 255}, (Vec3i){50, 50, 50}, 8, 2);
    memcpy(button.bg_hover_color, (Vec3i){50, 50, 50}, sizeof(Vec3i));
    memcpy(button.fg_hover_color, (Vec3i){255, 255, 255}, sizeof(Vec3i));

    std::chrono::system_clock::time_point time_start;
    std::chrono::system_clock::time_point time_end;
    time_start = std::chrono::high_resolution_clock::now();
    double delta_time;

    // Render Loop
    while(!glfwWindowShouldClose(window))
    {

	// Calculate delta time
	time_end = std::chrono::high_resolution_clock::now();
	delta_time = std::chrono::duration<double>(time_end-time_start).count();
	time_start = time_end;
	
	// Clear
	memset(pixel_map.data, 0, pixel_map.size * sizeof(RGBPixel));

	// Draw Top Bar
	draw_rectangle(&pixel_map, 800, 50, (Vec2i){0, 800-50}, (Vec3i){50, 50, 50});
	draw_sentence(&pixel_map, "snake maker", 8, 3, (Vec2i){30, 760}, (Vec3i){0, 255, 0});
	char buf[255];
	memset(buf, 0, 255);
	snprintf(buf, 255, "fps %.2lf", 1.0 / delta_time);
	draw_sentence(&pixel_map, buf, 8, 3, (Vec2i){550, 760}, (Vec3i){255, 0, 0});



	switch (game_state)
	{
	    case GameState::menu:
	    {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		ypos = win_height - ypos;

		draw_button(&pixel_map, &button, is_inside_collision_box(&button.col_box, (Vec2i){(int)xpos, (int)ypos}));
	    }

		break;
	    case GameState::snake:
		break;
	    case GameState::scoreboard:
		break;
	}

	// Drawing
	glTextureSubImage2D(tex, 0, 0, 0, pixel_map.width, pixel_map.height, GL_RGB, GL_UNSIGNED_BYTE, pixel_map.data);
	shader.use();
	glBindTextureUnit(0, tex);
	glUniform1i(glGetUniformLocation(shader.m_program, "u_tex"), 0);
	quad.draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    pixel_map_destroy(&pixel_map);

    return 0;
}


