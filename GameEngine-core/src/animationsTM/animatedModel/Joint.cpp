#include "Joint.h"

Joint::Joint(int index, std::string name, glm::mat4 bindLocalTransform) 
	: m_Index(index), m_Name(name), m_LocalBindTransform(bindLocalTransform)
{
}

Joint::~Joint()
{
}

void Joint::addChild(Joint child)
{
	m_Children.push_back(child);
}

void Joint::calcInverseBindTransform(glm::mat4 parentBindTransform)
{
	glm::mat4 bindTransform = parentBindTransform * m_LocalBindTransform;
	m_InverseBindTransform = glm::inverse(bindTransform);
	for (Joint child : m_Children) {
		child.calcInverseBindTransform(bindTransform);
	}
}
