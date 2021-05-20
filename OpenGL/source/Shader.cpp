#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "Shader.h"

Shader::Shader(const std::string& filepath) 
	: m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream stringStream[2];
    ShaderType type = ShaderType::NONE;
    //Loop until no more lines can be returned by getline
    while (getline(stream, line))
    {
        //if the current line contains #shader...
        if (line.find("#shader") != std::string::npos)
        {
            //...and if that shader is a vertex shader...
            if (line.find("vertex") != std::string::npos)
                //set the type to vertex
                type = ShaderType::VERTEX;
            //...or a fragment shader...
            else if (line.find("fragment") != std::string::npos)
                //set the type to fragment
                type = ShaderType::FRAGMENT;
        }
        else
        {
            //If the line does not contain #shader, it is actual shader code 
            //which is added to the stream along with a new line symbol.
            stringStream[(int)type] << line << "\n";
        }
    }

    //Returns both shader sources!
    return { stringStream[0].str(), stringStream[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    //Creates a shader with an id of id and compiles it
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    //If this shader fails to compile...
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        //... find out what is wrong...
        glGetShaderInfoLog(id, length, &length, message);
        //... and print it along with the shader type
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
        std::cout << message << std::endl;
        //No use in a shader with syntax errors!
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //Creates a new program out of the two shader sources
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //Sets up the program...
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //... and deletes the sources which are no longer needed
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}
void Shader::Unbind() const 
{
    glUseProgram(0);
}

//Functions for setting different types of uniforms
void Shader::SetUniform1i(const std::string& name, int v)
{
    glUniform1i(GetUniformLocation(name), v);
}
void Shader::SetUniform1f(const std::string& name, float v)
{
    glUniform1f(GetUniformLocation(name), v);
}
void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

//Finds the location of a uniform within the shader code
int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    //prints a warning if the wanted uniform does not exist
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' does not exist." << std::endl;
    //caches found uniforms to increase performance when looking them up again
    m_UniformLocationCache[name] = location;
    return location;
}