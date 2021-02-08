#include "includes/Texture.h"
#include <stb_image.h>
#include <iostream>
#include <glad/glad.h>

//a class to store texture data
//a texture is a data buffer that contains values which relate to pixel colours

Texture::Texture() :
	m_fileName(), m_width(0), m_height(0), m_textureID(0)
{}

Texture::~Texture()
{
	unload();
}

bool Texture::Load(std::string a_filePath) //loads the texture using the file path
{
	int width = 0, height = 0, channels = 0; //sets up dimensions
	stbi_set_flip_vertically_on_load(true); //flips the texture
	unsigned char* imageData = stbi_load(a_filePath.c_str(), &width, &height, &channels, 4);//loads image using dimensions
	if (imageData != nullptr)
	{
		m_fileName = a_filePath;
		m_width = width;
		m_height = height;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		//texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(imageData);
		std::cout << "successfully loaded image file: " << a_filePath << std::endl;
		return true;
	}
	std::cout << "failed to open file: " << a_filePath << std::endl;
	return false;
}

void Texture::unload()
{
	glDeleteTextures(1, &m_textureID); //deletes texture
}

unsigned int Texture::LoadCubeMap(std::vector<std::string> a_fileNames, unsigned int* cubeMap_face_id)
{
	//skybox
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	int width, height, nrChannels;
	for (int i = 0; i < 6; ++i)
	{
		unsigned char* data;
		stbi_set_flip_vertically_on_load(false); //dont flips
		data = stbi_load(a_fileNames[i].c_str(), &width, &height, &nrChannels, 4); //load image using demensions
		if (data != nullptr)
		{
			m_width = width;
			m_height = height;
			glTexImage2D(cubeMap_face_id[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			std::cout << "successfully loaded cube map file: " << a_fileNames[i] << std::endl;
		}
		else
		{
			std::cout << "failed to open file: " << a_fileNames[i] << std::endl;
		}
	}
	//texture parameters for cube map
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return m_textureID;
}
