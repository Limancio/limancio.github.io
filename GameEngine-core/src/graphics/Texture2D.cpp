#include "Texture2D.h"
#include <iostream>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

Texture2D::Texture2D()
	: mTexture(0), mSpecular(1) {}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTexture);
}

//-----------------------------------------------------------------------------
// Load a texture with a given filename using stb image loader
// http://nothings.org/stb_image.h
// Creates mip maps if generateMipMaps is true.
//-----------------------------------------------------------------------------
bool Texture2D::loadTexture(const string& fileName, GLuint& textureID, bool generateMipMaps)
{
	int width, height, components;

	// Use stbi image library to load our image
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &components, STBI_rgb_alpha);

	if (imageData == NULL)
	{
		std::cerr << "Error loading texture '" << fileName << "'" << std::endl;
		return false;
	}

	// Invert image
	int widthInBytes = width * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = height / 2;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageData + row * widthInBytes;
		bottom = imageData + (height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID); // all upcoming GL_TEXTURE_2D operations will affect our texture object (textureID)

											// Set the texture wrapping/filtering options (on the currently bound texture object)
											// GL_CLAMP_TO_EDGE
											// GL_REPEAT
											// GL_MIRRORED_REPEAT
											// GL_CLAMP_TO_BORDER
											// GL_LINEAR
											// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we don't accidentally mess up our textureID

	return true;
}

bool Texture2D::loadTexture(const string & textureFileName, const string & specularFileName)
{
	if (specularFileName.size() > 0) {
		loadTexture(textureFileName, mTexture, true);
		loadTexture(specularFileName, mSpecular, true);
	} else {
		loadTexture(textureFileName, mTexture, true);
		mSpecular = 0;
	}
		
	return false;
}

void Texture2D::bind(GLuint texUnit, bool useSpecular)
{
	if (useSpecular) {
		bind(texUnit, mTexture);
		bind(texUnit + 1, mSpecular);
	} else {
		bind(texUnit, mTexture);
	}
}

void Texture2D::unbind(GLuint texUnit, bool useSpecular)
{
	if (useSpecular) {
		unbind(texUnit);
		unbind(texUnit + 1);
	}
	else {
		unbind(texUnit);
	}
}

//-----------------------------------------------------------------------------
// Bind the texture unit passed in as the active texture in the shader
//-----------------------------------------------------------------------------
void Texture2D::bind(GLuint texUnit, GLuint& textureID)
{
	assert(texUnit >= 0 && texUnit < 32);

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

//-----------------------------------------------------------------------------
// Unbind the texture unit passed in as the active texture in the shader
//-----------------------------------------------------------------------------
void Texture2D::unbind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
