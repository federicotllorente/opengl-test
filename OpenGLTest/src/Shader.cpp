#include "Shader.h"
#include "GLHandleError.h"

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath)
	: m_Filepath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GL_CALL(glDeleteProgram(m_RendererID));
}

void Shader::Bind()
{
	GL_CALL(glUseProgram(m_RendererID));
}

void Shader::Unbind()
{
	GL_CALL(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GL_CALL(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                // Setting the mode/type to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                // Setting the mode/type to fragment
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    GL_CALL(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GL_CALL(glShaderSource(id, 1, &src, nullptr));
    GL_CALL(glCompileShader(id));

    int result;
    GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        char* message = (char*)alloca(sizeof(char) * length);
        GL_CALL(glGetShaderInfoLog(id, length, &length, message));

        Log("Failed to compile " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader!");
        Log(message);

        GL_CALL(glDeleteShader(id));
        return 0;
    }

    return id;
}

// TODO Move to constructor?
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GL_CALL(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GL_CALL(glAttachShader(program, vs));
    GL_CALL(glAttachShader(program, fs));
    GL_CALL(glLinkProgram(program));
    GL_CALL(glValidateProgram(program));

    /*glDetachShader(vs);
    glDetachShader(fs);*/

    GL_CALL(glDeleteShader(vs));
    GL_CALL(glDeleteShader(fs));

    return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GL_CALL(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' does not exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}
