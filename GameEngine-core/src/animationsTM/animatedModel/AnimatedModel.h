#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include <glm\glm.hpp>
#include <vector>
#include "../../models/Mesh.h"
#include "../../graphics/Texture2D.h"

#include "Joint.h"

class AnimatedModel
{
public:
	AnimatedModel(Mesh mesh, Texture2D texture, Joint rootJoint, int jointCount);
	~AnimatedModel();

	inline Mesh getModel() const { return m_Mesh; }
	inline Texture2D getTexture() const { return m_Texture; }
	inline Joint getRootJoint() const { return m_RootJoint; }

	void doAnimation(Animation animation) const;
	void update() const;
	std::vector<glm::mat4> getJointTransforms();
	void addJointsToArray(Joint headJoint, std::vector<glm::mat4> jointMatrices);

private:
	Mesh m_Mesh;
	Texture2D m_Texture;

	Joint m_RootJoint;
	int m_JointCount;

	Animator m_Animator;
};

#endif // !ANIMATED_MODEL_H
