#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "..\graphics\Texture2D.h"
#include "..\graphics\Material.h"
#include "glm/glm.hpp"


class Model {
public:

	Model() = default;
	//Model(const char* objFileName, const char* textFileName, Material material, bool useSpecular = false, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 ratation = glm::vec3(0.0f));
	~Model();

	inline void setScale(glm::vec3 newScale) { m_Scale = newScale; }
	inline void setRotation(glm::vec3 newRotation) { m_Rotation = newRotation; }
	inline glm::vec3 getScale() const { return m_Scale; }
	inline glm::vec3 getRotation() const { return m_Rotation; }

	inline Material getMaterial() const { return m_Material; }
	inline Texture2D getTexture() const { return m_Texture; }
	inline Mesh* getMesh() const { return m_ModelMesh; }
	inline bool useSpecular() const { return m_UseSpecular; }
	
	inline glm::vec3 getMinModel() const { return m_ModelMesh->getMin(); } //TODO: REMOVE
	inline glm::vec3 getMaxModel() const { return m_ModelMesh->getMax(); } //TODO: REMOVE

	void draw();
	void loadModel(const char* objFileName, const char* textFileName,
		Material material, glm::vec3 scale = glm::vec3(1.0f),
		glm::vec3 rotation = glm::vec3(0.0f), bool useSpecular = false, bool useFakeLight = false);
private:
	
	Mesh* m_ModelMesh;
	Texture2D m_Texture;
	Material m_Material;
	bool m_UseSpecular;

	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
};

#endif // !MODEL_H
