#ifndef JOINT_H
#define JOINT_H

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Joint
{
public:
	Joint(int index, std::string name, glm::mat4 bindLocalTransform);
	~Joint();

	void addChild(Joint child);

	inline glm::mat4 getAnimatedTransform() const { return m_AnimatedTransform; }
	inline void setAnimationTransform(glm::mat4 animationTransform) { m_AnimatedTransform = animationTransform; }

	inline glm::mat4 getInverseBindTransform() const { return m_InverseBindTransform; }

//protected:
	void calcInverseBindTransform(glm::mat4 parentBindTransform);

public:
	int m_Index;
	std::string m_Name;
	std::vector<Joint> m_Children;
private:
	glm::mat4 m_AnimatedTransform;
	glm::mat4 m_LocalBindTransform;
	glm::mat4 m_InverseBindTransform;

};


#endif // !JOINT_H
