#include "Terrain.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>


void Terrain::bindShader(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos)
{
	m_TerrainShader.enable();

	m_TerrainShader.setUniform1f("region1.max", m_TerrainRegions[0].max);
	m_TerrainShader.setUniform1f("region1.min", m_TerrainRegions[0].min);
							   					
	m_TerrainShader.setUniform1f("region2.max", m_TerrainRegions[1].max);
	m_TerrainShader.setUniform1f("region2.min", m_TerrainRegions[1].min);
							   					
	m_TerrainShader.setUniform1f("region3.max", m_TerrainRegions[2].max);
	m_TerrainShader.setUniform1f("region3.min", m_TerrainRegions[2].min);
							   					
	m_TerrainShader.setUniform1f("region4.max", m_TerrainRegions[3].max);
	m_TerrainShader.setUniform1f("region4.min", m_TerrainRegions[3].min);

	m_TerrainShader.setUniform1i("region1ColorMap", 0);
	m_TerrainShader.setUniform1i("region2ColorMap", 1);
	m_TerrainShader.setUniform1i("region3ColorMap", 2);
	m_TerrainShader.setUniform1i("region4ColorMap", 3);

	//m_TerrainShader.setUniform1i("multiplyTexture", 40);
	glm::mat4 model(1.0);
	model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f));

	m_TerrainShader.setUniformMat4("model", model);
	m_TerrainShader.setUniformMat4("view", view);
	m_TerrainShader.setUniformMat4("projection", projection);
	m_TerrainShader.setUniform3f("viewPos", viewPos);
	m_TerrainShader.setUniform1i("nrPointsLights", 1);
}

void Terrain::unbindShader()
{
	m_TerrainShader.disable();
}

Terrain::Terrain(int posX, int posZ, const char* shaderVSFileName, const char* shaderFSFileName)
	: m_Position(glm::vec2(posX * SIZE, posZ * SIZE)), m_FilePathTexture(), m_TerrainShader(shaderVSFileName, shaderFSFileName)
{
	m_TerrainRegions[0].min = -5.0f;
	m_TerrainRegions[0].max = 0.0f;
	m_TerrainRegions[0].texture.loadTexture("src/res/textures/terrain/dirt.jpg");

	m_TerrainRegions[1].min = 0.1f;
	m_TerrainRegions[1].max = 10.1f;
	m_TerrainRegions[1].texture.loadTexture("src/res/textures/terrain/rock.jpg");

	m_TerrainRegions[2].min = 10.2f;
	m_TerrainRegions[2].max = 20.3f;
	m_TerrainRegions[2].texture.loadTexture("src/res/textures/terrain/grass.jpg");

	m_TerrainRegions[3].min = 20.4f;
	m_TerrainRegions[3].max = 50.0f;
	m_TerrainRegions[3].texture.loadTexture("src/res/textures/terrain/snow.jpg");
	
	heights = new float[VERTEX_COUNT * VERTEX_COUNT];
	for (int i = 0; i < VERTEX_COUNT; i++)
		for (int j = 0; j < VERTEX_COUNT; j++)
			heights[j + i * VERTEX_COUNT] = 1.0f;
	 
	generateTerrain(false);
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &m_TerrainIBO);
	glDeleteVertexArrays(1, &m_TerrainVAO);
	glDeleteBuffers(1, &m_TerrainVBO);

}

void Terrain::drawTerrain(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos)
{
	bindShader(view, projection, viewPos);

	m_TerrainRegions[0].texture.bind(0, false);
	m_TerrainRegions[1].texture.bind(1, false);
	m_TerrainRegions[2].texture.bind(2, false);
	m_TerrainRegions[3].texture.bind(3, false);

	glBindVertexArray(m_TerrainVAO);
	glDrawElements(GL_TRIANGLES, (6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1)), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	m_TerrainRegions[0].texture.unbind(0, false);
	m_TerrainRegions[1].texture.unbind(1, false);
	m_TerrainRegions[2].texture.unbind(2, false);
	m_TerrainRegions[3].texture.unbind(3, false);

	unbindShader();
}

void updateBuffer(GLuint &id, GLuint offset, void *data, GLuint size, GLuint type) {
	glBindBuffer(type, id);
	glBufferSubData(type, offset, size, data);
}

void Terrain::setHeightOfTerrain(float worldX, float worldZ, float value, float radius)
{
	float terrainX = worldX - m_Position.x;
	float terrainZ = worldZ - m_Position.y;
	float gridSquareSize = SIZE / ((float)(VERTEX_COUNT) - 1);

	int gridX = (int)floor(worldX / gridSquareSize);
	int gridZ = (int)floor(worldZ / gridSquareSize);

	if (gridX >= ((float)VERTEX_COUNT) - 1 || gridZ >= ((float)VERTEX_COUNT) - 1 || gridX < 0 || gridZ < 0) {
		return;
	} 

	int startX = gridX - radius;
	int startZ = gridZ - radius;

	if (startX < 0)
		startX = 0;
	if (startZ < 0)
		startZ = 0;

	int endX = gridX + radius;
	int endZ = gridZ + radius;
	
	if (endX > VERTEX_COUNT)
		endX = VERTEX_COUNT;
	if (endZ < VERTEX_COUNT)
		endZ = VERTEX_COUNT;

	for (int x = startX; x < endX; x++) {
		for (int z = startZ; z < endZ; z++) {
			
			glm::vec2 indexPoint(x, z);
			glm::vec2 centerPoint(gridX, gridZ);

			float distance = radius - glm::distance(centerPoint, indexPoint);
			if (distance < 0)
				distance = 0;
			heights[x + z * VERTEX_COUNT] += distance * value;
		}

	}

	generateTerrain(true);
}

void Terrain::smoothHeightOfTerrain(float worldX, float worldZ, float value, float radius)
{
	float terrainX = worldX - m_Position.x;
	float terrainZ = worldZ - m_Position.y;
	float gridSquareSize = SIZE / ((float)(VERTEX_COUNT)-1);

	int gridX = (int)floor(worldX / gridSquareSize);
	int gridZ = (int)floor(worldZ / gridSquareSize);

	if (gridX >= ((float)VERTEX_COUNT) - 1 || gridZ >= ((float)VERTEX_COUNT) - 1 || gridX < 0 || gridZ < 0) {
		return;
	}

	int startX = gridX - radius;
	int startZ = gridZ - radius;

	if (startX < 0)
		startX = 0;
	if (startZ < 0)
		startZ = 0;

	int endX = gridX + radius;
	int endZ = gridZ + radius;

	if (endX > VERTEX_COUNT)
		endX = VERTEX_COUNT;
	if (endZ < VERTEX_COUNT)
		endZ = VERTEX_COUNT;

	for (int x = startX; x < endX; x++) {
		for (int z = startZ; z < endZ; z++) {

			glm::vec2 indexPoint(x, z);
			glm::vec2 centerPoint(gridX, gridZ);
			float newValue;

			float distance = radius - glm::distance(centerPoint, indexPoint);
			if (distance < 0)
				distance = 0;

			if (heights[x + z * VERTEX_COUNT] >= heights[gridX + gridZ * VERTEX_COUNT]) {
				newValue = value * -1;
			} else {
				newValue = value;
			}

			if (heights[x + z * VERTEX_COUNT] - 1.0f <= heights[gridX + gridZ * VERTEX_COUNT] && heights[x + z * VERTEX_COUNT] + 1.0f >= heights[gridX + gridZ * VERTEX_COUNT]) {
				if(distance != 0)
					heights[x + z * VERTEX_COUNT] = heights[gridX + gridZ * VERTEX_COUNT];
			} else {
				//if (x != gridX && z != gridZ)
					heights[x + z * VERTEX_COUNT] += distance * newValue;
			}
		}

	}

	generateTerrain(true);
}

glm::vec3 Terrain::calculateNormal(int x, int z)
{
	float heightL = heights[(x - 1) + z * VERTEX_COUNT];
	float heightR = heights[(x + 1) + z * VERTEX_COUNT];
	float heightD = heights[x + (z - 1) * VERTEX_COUNT];
	float heightU = heights[x + (z + 1) * VERTEX_COUNT];
	glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
	return glm::vec3(glm::normalize(normal));
}

float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::getHeightOfTerrain(float worldX, float worldZ) const
{
	float terrainX = worldX - m_Position.x;
	float terrainZ = worldZ - m_Position.y;
	float gridSquareSize = SIZE / ((float)(VERTEX_COUNT));

	int gridX = (int)floor(worldX / gridSquareSize);
	int gridZ = (int)floor(worldZ / gridSquareSize);

	if (gridX >= ((float)VERTEX_COUNT) - 1 || gridZ >= ((float)VERTEX_COUNT) - 1 || gridX < 0 || gridZ < 0) {
		return 0;
	}
	float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
	float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize;
	float answer;
	if (xCoord <= (1 - zCoord)) {
		answer = barryCentric(glm::vec3(0, heights[gridX + gridZ * VERTEX_COUNT], 0), glm::vec3(1,
				heights[(gridX + 1) + gridZ * VERTEX_COUNT], 0), glm::vec3(0,
					heights[gridX + (gridZ + 1) * VERTEX_COUNT], 1), glm::vec2(xCoord, zCoord));
	}
	else {
		answer = barryCentric(glm::vec3(1, heights[(gridX + 1) + gridZ * VERTEX_COUNT], 0), glm::vec3(1,
			heights[(gridX + 1) + (gridZ + 1) * VERTEX_COUNT], 1), glm::vec3(0,
				heights[gridX + (gridZ + 1) * VERTEX_COUNT], 1), glm::vec2(xCoord, zCoord));
	}
	return answer;
}

void Terrain::generateTerrain(bool updateBuff)
{
	float vertices[(VERTEX_COUNT * VERTEX_COUNT) * 3];
	float normals[(VERTEX_COUNT * VERTEX_COUNT) * 3];
	float textureCoords[(VERTEX_COUNT * VERTEX_COUNT) * 2];
	int indices[6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1)];
	int vertexPointer = 0;
	std::vector<VertexTerrain> mVertices;

	for (int i = 0; i<VERTEX_COUNT; i++) {
		for (int j = 0; j<VERTEX_COUNT; j++) {

			VertexTerrain meshVertex;

			meshVertex.position = glm::vec3((float)j / ((float)VERTEX_COUNT - 1) * SIZE,
				heights[j + i * VERTEX_COUNT],
				(float)i / ((float)VERTEX_COUNT - 1) * SIZE);

			//glm::vec3 normal = calculateNormal(j, i);
			meshVertex.normal = glm::vec3(calculateNormal(j, i));

			meshVertex.texCoords = glm::vec2((float)j / ((float)VERTEX_COUNT - 1), 
				(float)i / ((float)VERTEX_COUNT - 1));

			meshVertex.vertexHeight = getHeightOfTerrain(j, i);

			vertexPointer++;
			mVertices.push_back(meshVertex);
		}
	}
	int pointer = 0;
	for (int gz = 0; gz<VERTEX_COUNT - 1; gz++) {
		for (int gx = 0; gx<VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}
	 
	if (updateBuff) {
		updateBuffer(m_TerrainVBO, 0, &mVertices[0], mVertices.size() * sizeof(VertexTerrain), GL_ARRAY_BUFFER);
	}
	else {
		glGenVertexArrays(1, &m_TerrainVAO);				// Tell OpenGL to create new VertexTerrain Array Object
		glBindVertexArray(m_TerrainVAO);					// Make it the current one

		glGenBuffers(1, &m_TerrainVBO);					// Generate an empty VertexTerrain buffer on the GPU
		glBindBuffer(GL_ARRAY_BUFFER, m_TerrainVBO);		// "bind" or set as the current buffer we are working with
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(VertexTerrain)/* * sizeof(VertexTerrain)*/, &mVertices[0], GL_DYNAMIC_DRAW);

												// VertexTerrain Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &m_TerrainIBO);	// Create buffer space on the GPU for the index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TerrainIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)/* * sizeof(VertexTerrain)*/, indices, GL_STATIC_DRAW);

		// VertexTerrain Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// VertexTerrain Texture Coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTerrain), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// VertexTerrain height
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, (GLsizei) sizeof(VertexTerrain), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

		glEnableVertexAttribArray(0);

	}
}
