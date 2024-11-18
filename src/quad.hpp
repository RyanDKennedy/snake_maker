#pragma once

#include <glad/glad.h>

struct Quad
{
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    Quad();
    ~Quad();
    void draw();
};
