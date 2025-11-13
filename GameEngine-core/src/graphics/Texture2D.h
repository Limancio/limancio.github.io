//-----------------------------------------------------------------------------
#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <GL/glew.h>
#include <string>
using std::string;

class Texture2D
{
public:
	Texture2D();
	Texture2D(const Texture2D& rhs) {}
	~Texture2D();

	bool loadTexture(const string& textureFileName, const string& specularFileName = std::string());
	void bind(GLuint texUnit, bool useSpecular);
	void unbind(GLuint texUnit, bool useSpecular);

	inline GLuint getTexture() const { return mTexture; }
	inline GLuint getSpecular() const { return mSpecular; }

private:
	bool loadTexture(const string& fileName, GLuint& textureID, bool generateMipMaps = true);
	void bind(GLuint texUnit, GLuint& textureID);
	void unbind(GLuint texUnit = 0);
	Texture2D& operator = (const Texture2D& rhs) {}

	GLuint mTexture;
	GLuint mSpecular;
};
#endif //TEXTURE2D_H
