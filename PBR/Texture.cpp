#include "Texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <iostream>
#include <string>

unsigned int _texLoadTextureFromFile(const char *strPath, const char *strDirectory)
{
	std::string strFilename = std::string(strPath);
	strFilename = std::string(strDirectory) + '\\' + strFilename;

	std::cout << "LOG: Loading texture @" << strFilename << std::endl;

	GLuint iTextureID;
	glGenTextures(1, &iTextureID);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char *data = stbi_load(strFilename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		} else if (nrComponents == 3) {
			format = GL_RGB;
		} else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, iTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		std::cout << "ERR: Texture failed to load (@" << strPath << ")" << std::endl;
		glDeleteTextures(1, &iTextureID);
		iTextureID = -1;
	}
	stbi_image_free(data);

	return iTextureID;
}

CTexture::CTexture()
{
	for (int i = 0; i < 5; ++i) {
		tex_auiMaps[i] = -1;
	}
}

CTexture::~CTexture()
{
	for (int i = 0; i < 5; ++i) {
		glDeleteTextures(1, &tex_auiMaps[i]);
	}
}

bool CTexture::LoadTextures(const char *strPath, const char *strExtension)
{
	const std::string astr[] = { "albedo", "normal", "metallic", "roughness", "ao" };
	for (int i = 0; i < 5; ++i) {
		// load map
		std::string strName = astr[i] + std::string(strExtension);
		tex_auiMaps[i] = _texLoadTextureFromFile(strName.c_str(), strPath);
		// if something went wrong
		if (tex_auiMaps[i] == -1) {
			// stop loading and report an error
			return false;
		}
	}
	// everything was loaded correctly
	return true;
}

void CTexture::Render(void)
{
	for (int i = 0; i < 5; ++i) {
		glActiveTexture(GL_TEXTURE3 + i);
		glBindTexture(GL_TEXTURE_2D, tex_auiMaps[i]);
	}
}
