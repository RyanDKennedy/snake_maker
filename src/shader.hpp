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
    
    std::string m_vertex_path;
    std::string m_fragment_path;
    
    
    Shader(std::string vertex_path, std::string fragment_path);
    ~Shader();
    void use();
    GLuint get_program();
};
