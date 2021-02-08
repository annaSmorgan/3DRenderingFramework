#pragma once
#include <string>
#include <vector>

//a class to store texture data
//a texture is a data buffer that contains values which relate to pixel colours

class Texture
{
public:
	//constructor
	Texture();
	//destructor
	~Texture();

	//functions to load a texture from file
	bool Load(std::string a_fileName);
	void unload();
	//load the cube map
	unsigned int LoadCubeMap(std::vector<std::string> a_fileNames, unsigned int* cubeMap_face_id);
	//getters for texture details
	const std::string& GetFileName() const { return m_fileName; }
	unsigned int GetTextureID() const { return m_textureID; }
	void GetDimensions(unsigned int& a_w, unsigned int& a_h) const;

private:
	//variables
	std::string m_fileName;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_textureID;
};

inline void Texture::GetDimensions(unsigned int& a_w, unsigned int& a_h) const //gets the dimensions of tex
{
	a_w = m_width; a_h = m_height;
}