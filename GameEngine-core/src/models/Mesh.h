#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
//#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "glm/glm.hpp"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:

	Mesh();
	~Mesh();

	bool loadOBJ(const std::string& filename, bool useFakeLight);
	void draw();
	inline glm::vec3 getMin() const { return m_Min; }
	inline glm::vec3 getMax() const { return m_Max; }

private:

	void initBuffers();

	bool mLoaded;
	std::vector<Vertex> mVertices;
	glm::vec3 m_Min;
	glm::vec3 m_Max;
	GLuint mVBO, mVAO;
};
#endif //MESH_H
