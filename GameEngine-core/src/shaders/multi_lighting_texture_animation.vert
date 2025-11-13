#version 330 core


const int MAX_JOINTS = 50;//max joints allowed in a skeleton
const int MAX_WEIGHTS = 3;//max number of joints that can affect a vertex

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aJointIndices;
layout (location = 4) in vec2 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float Visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 jointTransforms[MAX_JOINTS];
uniform mat4 projectionViewMatrix;

const float density = 0.0035;
const float gradient = 5.0;

void main()
{
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	
    //FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  

	for(int i=0;i<MAX_WEIGHTS;i++){
		mat4 jointTransform = jointTransforms[in_jointIndices[i]];
		vec4 posePosition = jointTransform * vec4(aPos, 1.0);
		totalLocalPos += posePosition * in_weights[i];
		
		vec4 worldNormal = jointTransform * vec4(Normal, 0.0);
		totalNormal += worldNormal * in_weights[i];
	}
    
    vec4 worldPosition = model * vec4(FragPos, 1.0);
	vec4 positionRelativeToCam = view * worldPosition;

	float distance = length(positionRelativeToCam.xyz);
	Visibility = exp(-pow((distance * density), gradient));
	Visibility = clamp(Visibility, 0.0, 1.0);

    gl_Position = (projection * view) * totalLocalPos;
	Normal = totalNormal.xyz;
    TexCoords = aTexCoords;
}