#include "quad.hpp"

#include <cstdio>

Quad::Quad()
{
    GLfloat vertices[] =
    {
	// Position 3  Texture Coordinates 2
	0,  1, 0,     0, 1, // 0 TOP LEFT
	 1,  1, 0,     1, 1, // 1 TOP RIGHT
	0, 0, 0,     0, 0, // 2 BOTTOM LEFT
	 1, 0, 0,     1, 0, // 3 BOTTOM RIGHT
    };

    GLuint indices[] =
    {
        0, 3, 1, // TOP TRIANGLE
        0, 2, 3  // BOTTOM TRIANGLE
    };

    m_rotation = 0.0f;
    m_scale[0] = 1.0f;
    m_scale[1] = 1.0f;
    m_position[0] = 0.0f;
    m_position[1] = 0.0f;
    m_position[2] = 0.0f;
    update_model();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Quad::draw()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Quad::reset_model()
{
    m_rotation = 0.0f;
    m_scale[0] = 1.0f;
    m_scale[1] = 1.0f;
    m_position[0] = 0.0f;
    m_position[1] = 0.0f;
    m_position[2] = 0.0f;
    update_model();    
}

void Quad::update_model()
{
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, glm::vec3(m_position[0], m_position[1], m_position[2]));
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation), glm::vec3(0, 0, 1.0f));
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_scale[0], m_scale[1], 1.0f));
}

void Quad::translate(float x_trans, float y_trans, float z_trans)
{
    m_position[0] += x_trans;
    m_position[1] += y_trans;
    m_position[2] += z_trans;
}

void Quad::rotate(float rotation)
{
    m_rotation += rotation;
}

void Quad::scale(float x_scale, float y_scale)
{
    m_scale[0] *= x_scale;
    m_scale[1] *= y_scale;
}
	
