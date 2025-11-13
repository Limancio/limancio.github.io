#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>


	Shader::Shader(const char* vertPath, const char* fragPath)
		: m_VertPath(vertPath), m_FragPath(fragPath)
	{
		m_ShaderID = load();
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderID);
	}

	GLint Shader::getUniformLocation(const GLchar * name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];
		GLint location = glGetUniformLocation(m_ShaderID, name);
		m_UniformLocationCache[name] = location;
		return location;
	}

	GLuint Shader::load()
	{
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertSourceString = FileUtils::read_file(m_VertPath);
		std::string fragSourceString = FileUtils::read_file(m_FragPath);
		const char* vertSource = vertSourceString.c_str();
		const char* fragSource = fragSourceString.c_str();

		glShaderSource(vertex, 1, &vertSource, NULL);
		glCompileShader(vertex);

		GLint result;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			GLint length;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(vertex, length, &length, &error[0]);
			std::cout << "Failed to compile vertex shader! Error message:" << std::endl << &error[0] << std::endl;
			glDeleteShader(vertex);
			return 0;
		}

		glShaderSource(fragment, 1, &fragSource, NULL);
		glCompileShader(fragment);


		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			GLint length;
			glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(fragment, length, &length, &error[0]);
			std::cout << "Failed to compile fragment shader! Error message:" << std::endl << &error[0] << std::endl;
			glDeleteShader(fragment);
			return 0;
		}

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);

		glLinkProgram(program);
		glValidateProgram(program);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		m_UniformLocationCache.clear();

		return program;
	}

	void Shader::enable() const {
		glUseProgram(m_ShaderID);
	}

	void Shader::disable() const {
		//glUseProgram(0);
	}
	void Shader::setUniform1f(const GLchar * name, float value)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform1f(location, value);
	}
	void Shader::setUniform1i(const GLchar * name, int value)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform1i(location, value);
	}
	void Shader::setUniformSampler(const GLchar * name, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform1i(location, slot);
	}
	void Shader::setUniform2f(const GLchar * name, const glm::vec2 & vector)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform2f(location, vector.x, vector.y);
	}
	void Shader::setUniform3f(const GLchar * name, const glm::vec3 & vector)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform3f(location, vector.x, vector.y, vector.z);
	}
	void Shader::setUniform4f(const GLchar * name, const glm::vec4 & vector)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
	}
	void Shader::setUniformMat4(const GLchar * name, const glm::mat4 & matrix)
	{
		GLint location = getUniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}