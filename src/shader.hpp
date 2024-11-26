#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include "utils.hpp"

class Shader
{
public:

    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_program;
    
    Shader(const char *vertex_source_cstr, const char *fragment_source_cstr);
    ~Shader();
    void use();
    GLuint get_program();
};
