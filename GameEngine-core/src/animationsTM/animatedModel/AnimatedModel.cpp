#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(Mesh mesh, Texture2D texture, Joint rootJoint, int jointCount)
	: m_Mesh(mesh), m_Texture(texture), m_RootJoint(rootJoint), m_JointCount(jointCount), m_Animator(*this)
{
	m_RootJoint.calcInverseBindTransform(glm::mat4(0.0f));
}

AnimatedModel::~AnimatedModel()
{
	m_Mesh.~Mesh();
	m_Texture.~Texture2D();
}

void AnimatedModel::doAnimation(Animation animation) const
{
	animation.doAnimation(animation);
}

void AnimatedModel::update() const
{
	m_Animator.update();
}

std::vector<glm::mat4> AnimatedModel::getJointTransforms()
{
	std::vector<glm::mat4> jointMatrices;
	addJointsToArray(m_RootJoint, jointMatrices);
	return jointMatrices;
}

void AnimatedModel::addJointsToArray(Joint headJoint, std::vector<glm::mat4> jointMatrices)
{
	jointMatrices[headJoint.m_Index] = headJoint.getAnimatedTransform();
	for(Joint childJoint : headJoint.m_Children) {
		addJointsToArray(childJoint, jointMatrices);
	}
}
