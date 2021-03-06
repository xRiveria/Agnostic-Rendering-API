#include "GAAPrecompiledHeader.h"
#include "LearnShader.h"
#include "GL/glew.h"

LearnShader::LearnShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	//Retrieve the vertex/fragment source code form filepath.
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//Ensure ifstream objects can throw exceptions.
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//Open Files
		vShaderFile.open(vertexShaderPath);
		fShaderFile.open(fragmentShaderPath);
		std::stringstream vShaderStream, fShaderStream;
		//Read file's buffer contents into streams.
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//Close file handlers.
		vShaderFile.close();
		fShaderFile.close();
		//Convert stream into string.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}

	catch(std::ifstream::failure e)
	{
		std::cout << "Error::Shader::File_Not_Successfully_Read" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//2. Compile Shaders
	unsigned int vertex, fragment;
	//Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	//Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	//Shader Program
	m_ShaderID = glCreateProgram();
	glAttachShader(m_ShaderID, vertex);
	glAttachShader(m_ShaderID, fragment);
	glLinkProgram(m_ShaderID);
	CheckCompileErrors(m_ShaderID, "PROGRAM");

	//Delete the shaders once they've been linked into our program.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void LearnShader::UseShader()
{
	glUseProgram(m_ShaderID);
}

void LearnShader::DeleteShader()
{
	glDeleteProgram(m_ShaderID);
}

void LearnShader::SetUniformFloat(const std::string& name, float value) const
{
	unsigned int uniformLocation = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform1f(uniformLocation, value);
}

void LearnShader::SetUniformInteger(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}

void LearnShader::SetUniformBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), (int)value);
}

void LearnShader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_ERROR_OF_TYPE: " << type << "\n" << infoLog;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING:ERROR::OF::TYPE: " << type << "\n" << infoLog;
		}
	}
}

