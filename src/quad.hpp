#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "math_types.hpp"

struct Quad
{
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    Vec3f m_position;
    Vec2f m_scale;
    GLfloat m_rotation;

    glm::mat4 m_model_matrix;

    Quad();
    ~Quad();
    void draw();

    // SCALE -> ROTATE -> TRANSLATE
    void update_model();
    void translate(float x_trans, float y_trans, float z_trans);
    void rotate(float rotation);
    void scale(float x_scale, float y_scale);
    void reset_model();
    
};
