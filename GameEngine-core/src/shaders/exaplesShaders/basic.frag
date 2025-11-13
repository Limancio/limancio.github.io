#version 330 core

in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 lightColor;

void main()
{

	vec4 texCoord = vec4(lightColor, 1.0f);
	frag_color = texCoord;
}
