#include "Model.h"
/*
Model::Model(const char * objFileName, const char * textFileName, 
	Material material, bool useSpecular, glm::vec3 scale, glm::vec3 rotation)
	: m_ModelMesh(), m_Material(material), m_UseSpecular(useSpecular), m_Scale(scale), m_Rotation(rotation)
{
}
*/
Model::~Model()
{
}

void Model::draw()
{
	m_Texture.bind(0, m_UseSpecular);
	m_ModelMesh->draw();
	m_Texture.unbind(0, m_UseSpecular);
}

void Model::loadModel(const char * objFileName, const char * textFileName, Material material, glm::vec3 scale, glm::vec3 rotation, bool useSpecular, bool useFakeLight)
{
	m_Scale = scale;
	m_Rotation = rotation;
	m_UseSpecular = useSpecular;
	m_Material = material;
	m_ModelMesh = new Mesh();
	m_ModelMesh->loadOBJ(objFileName, useFakeLight);

	if (m_UseSpecular) {
		std::string specularFile;
		specularFile.append(textFileName);
		std::string path = specularFile.substr(0, specularFile.find("."));
		path.append("_specular");
		std::string endfile = specularFile.substr(specularFile.find("."), path.size());
		path.append(endfile);
		m_Texture.loadTexture(textFileName, path);
	}
	else {
		m_Texture.loadTexture(textFileName);
	}
}
