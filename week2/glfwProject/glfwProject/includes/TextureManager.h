#pragma once
#include <map>
#include <string>
//forward declare texture as we only need to keep a pointer here
//and this avoids cyclic dependency
class Texture;
//class that points to texture and manages the textures uses
class TextureManager
{
public:
	// this manager class will act as a singleton object for ease of access
	static TextureManager* CreateInstance();
	static TextureManager* GetInstance();
	static void DestroyInstance();

	bool TextureExists(const char* a_pName);
	//load a texture from file --> calls texture::load()
	unsigned int LoadTexture(const char* a_pfileName);
	//gets the texture
	unsigned int GetTexture(const char* a_fileName);
	//realses the texture
	void ReleaseTexture(unsigned int a_texture);

private:

	static TextureManager* m_instance;
	//a small structure to reference count a texture
	//reference count indicates how many points are
	//currently pointing to this texture -> only unload at 0 refs
	typedef struct TextureRef
	{
		Texture* pTexture;
		unsigned int refCount;
	}TextureRef;
	//texture map
	std::map<std::string, TextureRef> m_pTextureMap;
	//constructor and destructor 
	TextureManager();
	~TextureManager();
};
