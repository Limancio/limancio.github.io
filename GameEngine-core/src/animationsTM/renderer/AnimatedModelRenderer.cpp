#include "AnimatedModelRenderer.h"

AnimatedModelRenderer::AnimatedModelRenderer(const char * animatorShaderVS, const char * animatorShaderFS)
	: m_Shader(animatorShaderVS, animatorShaderFS)
{
}
