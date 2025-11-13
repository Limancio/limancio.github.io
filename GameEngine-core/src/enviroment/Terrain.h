#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include "..\graphics\Texture2D.h"
#include "..\graphics\Shader.h"

const static int SIZE = 128;
const static int VERTEX_COUNT = 128;
const static float HEIGHTMAP_SCALE = 2.0f;
const static int TERRAIN_REGIONS_COUNT = 4;

struct TerrainRegion
{

	TerrainRegion() : min(-1), max(-1), texture(Texture2D()) {};
	float min;
	float max;
	Texture2D texture;
};

struct VertexTerrain
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	float vertexHeight;
};

class Terrain {

private:
	void bindShader(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos);
	void unbindShader();

public:
	Terrain(int posX, int posZ, const char* shaderVSFileName, const char* shaderFSFileName);
	~Terrain();
	void drawTerrain(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos);
	
	void setHeightOfTerrain(float worldX, float worldZ, float value, float radius);
	void smoothHeightOfTerrain(float worldX, float worldZ, float value, float radius);
	
	glm::vec3 calculateNormal(int x, int z);
	//inline float getHeight(int x, int z) { if (x >= 0 && z >= 0 && x <= VERTEX_COUNT && z <= VERTEX_COUNT) return heights[x + z * VERTEX_COUNT]; else - 1000000; }
	inline Shader* getShader() { return (&m_TerrainShader); }

	float getHeightOfTerrain(float worldX, float worldZ) const;

private:
	void generateTerrain(bool updateBuff);

	GLuint m_TerrainVAO, m_TerrainVBO, m_TerrainIBO;

	Shader m_TerrainShader;
	float* heights;
	glm::vec2 m_Position;
	const char* m_FilePathTexture;

	TerrainRegion m_TerrainRegions[TERRAIN_REGIONS_COUNT];

};

#endif // !TERRAIN_H
