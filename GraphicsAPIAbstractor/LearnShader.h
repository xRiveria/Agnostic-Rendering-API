#pragma once

class LearnShader
{
public:
	LearnShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void UseShader();
	void DeleteShader();
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformInteger(const std::string& name, int value) const;
	void SetUniformBool(const std::string& name, bool value) const;
	inline unsigned int GetShaderID() const { return m_ShaderID; }

private:
	void CheckCompileErrors(unsigned int shader, std::string type);
	unsigned int m_ShaderID;
};