#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>

#include "../utils/fileutils.h"
#include <glm\glm.hpp>


	class Shader {
	private:
		GLuint m_ShaderID;
		const char* m_VertPath;
		const char* m_FragPath;
		std::unordered_map<std::string, GLint> m_UniformLocationCache;

	public:
		Shader(const char* vertPath, const char* fragPath);
		~Shader();

		void enable() const;
		void disable() const;


		void setUniform1f(const GLchar* name, float value);
		void setUniform1i(const GLchar* name, int value);
		void setUniformSampler(const GLchar* name, int slot);
		void setUniform2f(const GLchar* name, const glm::vec2& vector);
		void setUniform3f(const GLchar* name, const glm::vec3& vector);
		void setUniform4f(const GLchar* name, const glm::vec4& vector);

		void setUniformMat4(const GLchar* name, const glm::mat4& matrix);
		inline GLuint getShaderID() { return this->m_ShaderID; };

	private:
		GLint getUniformLocation(const GLchar* name);
		GLuint load();
	};