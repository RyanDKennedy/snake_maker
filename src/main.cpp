#include <cstdio>
#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "drawing.hpp"
#include "shader.hpp"
#include "quad.hpp"
#include "pixel.hpp"
#include "pixel_map.hpp"
#include "custom_fonts.hpp"

int main(void)
{
    const int win_width = 400;
    const int win_height = 400;

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

    float clear_color[4] = {0.0, 0.0, 0.0, 0.0};

    Shader shader("../shaders/quad.vert", "../shaders/quad.frag");
    Quad quad;

    // Pixel Map Settings

    PixelMap pixel_map = pixel_map_create(win_width, win_height);

    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);

    float border_color[] = {0.0, 0.0, 0.0, 0.0};
    glTextureParameterfv(tex, GL_TEXTURE_BORDER_COLOR, border_color);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureStorage2D(tex, 1, GL_RGBA8, pixel_map.width, pixel_map.height);

    init_fonts();

    Vec2i pos = {50, 370};
    draw_sentence(&pixel_map, "snake maker", 8, 2, pos);

    Vec2i pos1 = {0, 50};
    draw_sentence(&pixel_map, "abcdefghijklmnopqrstuvwxyz", 8, 1, pos1);


    // Render Loop
    while(!glfwWindowShouldClose(window))
    {
	// Clear
        glClearBufferfv(GL_COLOR, 0, clear_color);

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


