#include "AnimModel.h"

AnimModel::AnimModel()
{
	m_JointArray = nullptr;
	m_MeshArray = nullptr;
	m_FinalMeshArray = nullptr;
	
	m_InvBindPose = nullptr;
	m_SkinnedMatrices = nullptr;

	m_VertexArrayIds = nullptr;
	m_VertexBufferIds = nullptr;
	m_IndexBufferIds = nullptr;
	m_NumIndices = nullptr;
}

AnimModel::~AnimModel()
{
}

bool AnimModel::loadMesh(const char * fileNameMesh)
{
	std::ifstream file(fileNameMesh);
	if (!file.is_open()) {
		return false;
	}

	int initpos = file.tellg();
	file.seekg(0, std::ios::end);
	int fileLength = file.tellg();
	file.seekg(initpos);

	int iMesh = 0;
	std::string param, junk;

	file >> param;

	while (!file.eof()) {
		if(param == "MDS5Version") {
			file >> m_AnimVersion;
			if (m_AnimVersion != 10)
				return false;

			printf("MD5Version: %i\n", m_AnimVersion);
		}
		else if (param == "commandline") {
			file.ignore(fileLength, '\n');
		}
		else if(param == "numJoints") {
			file >> m_NumJoints;
			m_JointArray = new AnimJoint[m_NumJoints];
			if (!m_JointArray)
				return false;

			m_InvBindPose = new glm::mat4[m_NumJoints];
			if (!m_InvBindPose)
				return false;
			
			m_SkinnedMatrices = new glm::mat4[m_NumJoints];
			if (!m_SkinnedMatrices)
				return false;
		}
		else if (param == "numMeshes") {
			file >> m_NumMeshes;
			m_MeshArray = new AnimMesh[m_NumMeshes];
			if (!m_MeshArray)
				return false;
			m_FinalMeshArray = new OglMesh[m_NumMeshes];
			if (!m_FinalMeshArray)
				return false;
		}
		else if (param == "joints") {
			file >> junk; // {

			for (int i = 0; i < m_NumJoints; i++) {
				AnimJoint joint;
				file >> joint.name >> joint.parentId >> junk; // (
				file >> joint.position[0] >> joint.position[1] >> joint.position[2];
				file >> junk >> junk; // )(
				file >> joint.orientation[0] >> joint.orientation[1] >> joint.orientation[2];
				file >> junk; // )

				m_JointArray[i] = joint;
			}

			file >> junk; // }
		}
		else if (param == "mesh") {
			file >> junk; // {

			AnimMesh mesh;

			file >> param;
			while (param != "}") {
				if (param == "shader")
				{
					file >> mesh.shader;
					FileUtils::RemoveQuotes(mesh.shader);

					std::string texture = mesh.shader + '\n';
					printf(texture.c_str());
				}
				else if (param == "numverts")
				{
					file >> mesh.numVerts;
					mesh.vertexArray = new AnimVertex[mesh.numVerts];
					if (!mesh.vertexArray)
						return false;

					for (int i = 0; i < mesh.numVerts; i++)
					{
						AnimVertex vert;
						file >> junk >> junk >> junk; //vert vertIdx (
						file >> vert.texCoord[0] >> vert.texCoord[1] >> junk; // )
						file >> vert.startWeight >> vert.numWeights;

						mesh.vertexArray[i] = vert;
					}
				}
				else if (param == "numtris")
				{
					file >> mesh.numTris;
					mesh.triangleArray = new AnimTriangle[mesh.numTris];
					if (!mesh.triangleArray)
						return false;

					for (int i = 0; i < mesh.numTris; i++)
					{
						AnimTriangle tri;
						file >> junk >> junk; //tri triIdx
						file >> tri.indices[0] >> tri.indices[1] >> tri.indices[2];

						mesh.triangleArray[i] = tri;
					}
				}
				else if (param == "numweights")
				{
					file >> mesh.numWeights;
					mesh.weightArray = new AnimWeight[mesh.numWeights];
					if (!mesh.weightArray)
						return false;

					for (int i = 0; i < mesh.numWeights; i++)
					{
						AnimWeight weight;
						file >> junk >> junk; //weight weightIdx
						file >> weight.jointId >> weight.bias >> junk; // (
						file >> weight.position[0] >> weight.position[1] >> weight.position[2];
						file >> junk; // )

						mesh.weightArray[i] = weight;
					}
				}

				file >> param;
			}

			m_MeshArray[iMesh] = mesh;
			iMesh++;

		}

		file >> param;

	}

	return true;
}

void AnimModel::shutdownMesh()
{
	if (m_JointArray)
	{
		delete[] m_JointArray;
		m_JointArray = NULL;
	}
	if (m_MeshArray)
	{
		for (int i = 0; i < m_NumMeshes; i++)
		{
			delete[] m_MeshArray[i].vertexArray;
			delete[] m_MeshArray[i].triangleArray;
			delete[] m_MeshArray[i].weightArray;
			m_MeshArray[i].vertexArray = NULL;
			m_MeshArray[i].triangleArray = NULL;
			m_MeshArray[i].weightArray = NULL;
		}
		delete[] m_MeshArray;
		m_MeshArray = NULL;
	}
	if (m_FinalMeshArray)
	{
		for (int i = 0; i < m_NumMeshes; i++)
		{
			///			delete[] m_pFinalMeshArray[i].vertexArray;
			//			delete[] m_pFinalMeshArray[i].indexArray;
			//		m_pFinalMeshArray[i].vertexArray = NULL;
			//		m_pFinalMeshArray[i].indexArray = NULL;
		}
		delete[] m_FinalMeshArray;
		m_FinalMeshArray = NULL;
	}
}

void AnimModel::computeVertexPositionsFromWeights(AnimMesh *, OglMesh *)
{
	glm::mat4 rotation, rotationInv, translationInv, finalmatrix;
	for (int i = 0; i < m_NumJoints; i++)
	{
		AnimJoint& joint = m_JointArray[i];

		glm::toMat4(glm::quat(rotation, joint.orientation));
		mlQuaternionToMat4(rotation, joint.orientation);
		mlTransposeMat4(rotationInv, rotation);
		mlTranslationMat4(translationInv, -joint.position[0], -joint.position[1], -joint.position[2]);

		mlMultiplyMat4_2(finalmatrix, rotationInv, translationInv);
		memcpy(m_pInvBindPose[i], finalmatrix, sizeof(mat4));
	}
}
