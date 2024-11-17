#include "shader.hpp"

Shader::Shader(const char *vertex_source_cstr, const char *fragment_source_cstr)
{
    m_program = glCreateProgram();

    m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    bool error = false;

    glShaderSource(m_vertex_shader, 1, &vertex_source_cstr, NULL);
    glShaderSource(m_fragment_shader, 1, &fragment_source_cstr, NULL);

    GLint success;
    GLchar infolog[512];

    glCompileShader(m_vertex_shader);
    glCompileShader(m_fragment_shader);

    glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_vertex_shader, 512, NULL, infolog);
        std::cout << "\nERROR: Vertex shader failed to compile:\n\t" << infolog << std::endl;
    }
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_fragment_shader, 512, NULL, infolog);
        std::cout << "\nERROR: Fragment shader failed to compile:\n\t" << infolog << std::endl;
    }

    glAttachShader(m_program, m_vertex_shader);
    glAttachShader(m_program, m_fragment_shader);

    glLinkProgram(m_program);
}

GLuint Shader::get_program()
{
    return m_program;
}

Shader::~Shader()
{
    glDeleteShader(m_vertex_shader);
    glDeleteShader(m_fragment_shader);
    glDeleteProgram(m_program);
}

void Shader::use()
{
    glUseProgram(m_program);
}
