#include "shader.hpp"

Shader::Shader(std::string vertex_path, std::string fragment_path)
{
    m_program = glCreateProgram();

    m_vertex_path = vertex_path;
    m_fragment_path = fragment_path;

    m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    bool error = false;

    std::string vertex_source = rdk::read_file(m_vertex_path, error);
    if (error)
    {
	printf("ERROR: shader()\n\tFailed to open file\n\t%s\n", m_vertex_path.c_str());
	exit(1);
    }
    const char* vertex_source_cstr = vertex_source.c_str();

    std::string fragment_source = rdk::read_file(m_fragment_path, error);
    if (error)
    {
	printf("ERROR: shader()\n\tFailed to open file\n\t%s\n", m_fragment_path.c_str());
	exit(1);
    }
    const char* fragment_source_cstr = fragment_source.c_str();

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
        std::cout << "\nERROR: Vertex shader failed to compile:\n\t" << infolog << m_vertex_path << std::endl;
    }
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_fragment_shader, 512, NULL, infolog);
        std::cout << "\nERROR: Fragment shader failed to compile:\n\t" << infolog << m_fragment_path << std::endl;
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
