#ifndef ANIMATED_MODEL_RENDERER_H
#define ANIMATED_MODEL_RENDERER_H

#include "../../graphics/Shader.h"

class AnimatedModelRenderer
{
private:
	void bindShader(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor);
	void unbindShader();
public:
	AnimatedModelRenderer(const char* animatorShaderVS, const char* animatorShaderFS);
	~AnimatedModelRenderer();

	void draw(AnimatedModel entity, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor);

	inline Shader getShader() const { return m_Shader; };

private:
	Shader m_Shader;
};

#endif // !ANIMATED_MODEL_RENDERER_H
