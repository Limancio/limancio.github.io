#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>

std::vector<std::string> split(std::string s, std::string t) {

	std::vector<std::string> result;
	while (1) {
		int pos = s.find(t);
		if (pos == -1) {
			result.push_back(s);
			break;
		}
		result.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size() - pos - 1);

	}
	return result;
}

Mesh::Mesh()
	:mLoaded(false)
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

bool Mesh::loadOBJ(const std::string& filename, bool useFakeLight)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;


	if (filename.find(".obj") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			std::stringstream ss(lineBuffer);
			std::string cmd;
			int dim = 0;
			ss >> cmd;

			if (cmd == "v")
			{
				glm::vec3 vertex;
				while (dim < 3 && ss >> vertex[dim])
					dim++;
				
				tempVertices.push_back(vertex);
			}
			else if (cmd == "vt")
			{
				glm::vec2 uv;
				while (dim < 2 && ss >> uv[dim])
					dim++;
				tempUVs.push_back(uv);
			}
			else if (cmd == "vn")
			{
				glm::vec3 normal;
				while (dim < 3 && ss >> normal[dim])
					dim++;
				normal = glm::normalize(normal);
				tempNormals.push_back(normal);
			}
			else if (cmd == "f")
			{
				std::string faceData;
				int vertexIndex, uvIndex, normalIndex;

				while (ss >> faceData) {
					std::vector<std::string> data = split(faceData, "/");

					// vertex index
					if (data[0].size() > 0) {
						sscanf_s(data[0].c_str(), "%d", &vertexIndex);
						vertexIndices.push_back(vertexIndex);
					}
					// if the face vertex has a texture coordinate index
					if (data.size() >= 1) {
						if (data[1].size() > 0) {
							sscanf_s(data[1].c_str(), "%d", &uvIndex);
							uvIndices.push_back(uvIndex);
						}
					}

					// if the face vertex has a normal index
					if (data.size() >= 2) {
						if (data[2].size() > 0) {
							sscanf_s(data[2].c_str(), "%d", &normalIndex);
							normalIndices.push_back(normalIndex);
						}
					}

				}
			}
		}

		// Close the file
		fin.close();

		glm::vec3 min(0.0f);
		glm::vec3 max(0.0f);
		
		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex meshVertex;
			if (tempVertices.size() > 0) {
				glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];

				if (min.x > vertex.x) 
					min.x = vertex.x;
				if (min.y > vertex.y)
					min.y = vertex.y;
				if (min.z > vertex.z)
					min.z = vertex.z;

				if (max.x < vertex.x)
					max.x = vertex.x;
				if (max.y < vertex.y)
					max.y = vertex.y;
				if (max.z < vertex.z)
					max.z = vertex.z;

				meshVertex.position = vertex;
			}

			if (tempNormals.size() > 0) {
				if (useFakeLight) {
					meshVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
				} else {
					glm::vec3 normal = tempNormals[normalIndices[i] - 1];
					meshVertex.normal = normal;
				}
			}

			if (tempUVs.size() > 0) {
				glm::vec2 uv = tempUVs[uvIndices[i] - 1];
				meshVertex.texCoords = uv;
			}

			mVertices.push_back(meshVertex);
		}

		std::cout << filename << ": Min [" << min.x << ", " << min.y << ", " << min.z << "]" << std::endl;
		std::cout << filename << ": Max [" << max.x << ", " << max.y << ", " << max.z << "]" << std::endl;
		m_Min = min;
		m_Max = max;
		// Create and initialize the buffers
		initBuffers();

		return (mLoaded = true);
	}

	// We shouldn't get here so return failure
	return false;
}

void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Vertex Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

void Mesh::draw()
{
	if (!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mVertices.size());
	glBindVertexArray(0);
}

