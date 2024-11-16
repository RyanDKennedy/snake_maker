#pragma once

#include <glad/glad.h>

struct Quad
{
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    Quad();
    ~Quad();
    void update_model();
    void draw();
    
    void translate(float x_trans, float y_trans);
    void rotate(float rotation);
    void scale(float x_scale, float y_scale);
};
