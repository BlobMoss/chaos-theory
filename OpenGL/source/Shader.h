#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

//A struct that allows both shader sources to be returned at once
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Functions for setting different types of uniforms
	void SetUniform1i(const std::string& name, int v);
	void SetUniform1f(const std::string& name, float v);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	//Converts a file to two program sources
	ShaderProgramSource ParseShader(const std::string& filepath);
	//Compiles and prints any compiler errors
	unsigned int CompileShader(unsigned int type, const std::string& source);
	//Attaches the vertex and fragment shader to OpenGl
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	//Finds the location of a uniform within a shader
	int GetUniformLocation(const std::string& name);
};