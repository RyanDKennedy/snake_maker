#include "quad.hpp"


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

    glCreateVertexArrays(1, &m_VAO);
    glCreateBuffers(1, &m_VBO);
    glCreateBuffers(1, &m_EBO);

    glNamedBufferData(m_VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(m_EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribBinding(m_VAO, 0, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(m_VAO, 1);
    glVertexArrayAttribBinding(m_VAO, 1, 0);
    glVertexArrayAttribFormat(m_VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, 5 * sizeof(vertices[0]));
    glVertexArrayElementBuffer(m_VAO, m_EBO);
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

