#ifndef ANIM_MODEL_H
#define ANIM_MODEL_H

#include <glm\glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <fstream>
#include <istream>

#include "../graphics/Texture2D.h"
#include "../utils/fileutils.h"

struct OglVertex {
	glm::vec3 position;
	glm::vec2 texDoord;
	glm::vec4 indices;
	glm::vec4 weights;
};

struct OglMesh {
	std::string texture;
	OglVertex* vertexArray;
	unsigned int* indexArray;
	unsigned int numVertices;
	unsigned int numIndices;
};

struct AnimVertex {
	glm::vec2 texCoord;
	int startWeight;
	int numWeights;
};

struct AnimTriangle {
	int indices[3];
};

struct AnimWeight {
	int jointId;
	float bias;
	glm::vec3 position;
};

struct AnimJoint {
	std::string name;
	int parentId;
	glm::vec3 position;
	glm::vec4 orientation;
};

struct AnimMesh {
	std::string shader;
	AnimVertex* vertexArray;
	AnimTriangle* triangleArray;
	AnimWeight* weightArray;
	int numVerts;
	int numTris;
	int numWeights;
};

class AnimModel {
public:
	AnimModel();
	~AnimModel();

	bool init(const char* fileNameMesh, const char* fileNameAnim);
	void shutdown();

	void update() const;
	void render() const;
	glm::mat4 getSkinnedMatrices();

private:
	bool loadMesh(const char* fileNameMesh);
	void shutdownMesh();
	void buildBindPose();
	void computeVertexPositionsFromWeights(AnimMesh*, OglMesh*);
	bool prepareMeshes();

	bool loadAnim(const char*);
	void shutdown();

	bool initBuffers();
	void shutdownBuffers();
	bool initTextures();
	void shutdownTextures();

private:
	int m_AnimVersion;
	int m_NumJoints;
	int m_NumMeshes;

	AnimJoint* m_JointArray;
	AnimMesh* m_MeshArray;
	OglMesh* m_FinalMeshArray;
	
	glm::mat4* m_InvBindPose;
	glm::mat4* m_SkinnedMatrices;

	//OpenGL

	unsigned int* m_VertexArrayIds;
	unsigned int* m_VertexBufferIds;
	unsigned int* m_IndexBufferIds;
	unsigned int* m_NumIndices;

	Texture2D** m_Textures;
};

#endif // !ANIM_MODEL_H
