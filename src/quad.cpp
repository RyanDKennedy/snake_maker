#include "quad.hpp"

#include <cstdio>

Quad::Quad()
{
    GLfloat vertices[] =
    {
	// Position 3  Texture Coordinates 2
	-1,  1, 0,     0, 1, // 0 TOP LEFT
	 1,  1, 0,     1, 1, // 1 TOP RIGHT
	-1, -1, 0,     0, 0, // 2 BOTTOM LEFT
	 1, -1, 0,     1, 0, // 3 BOTTOM RIGHT
    };

    GLuint indices[] =
    {
        0, 3, 1, // TOP TRIANGLE
        0, 2, 3  // BOTTOM TRIANGLE
    };

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

