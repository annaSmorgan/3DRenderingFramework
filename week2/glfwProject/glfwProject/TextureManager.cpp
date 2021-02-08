#include "includes/TextureManager.h"
#include "includes/Texture.h"

//class that points to texture and manages the textures uses

//set up static pointer for singleton object
TextureManager* TextureManager::m_instance = nullptr;

TextureManager* TextureManager::CreateInstance()
{
	if (nullptr == m_instance) //if no instance
	{
		m_instance = new TextureManager(); //create instance
	}
	return m_instance; //return instance
}

TextureManager* TextureManager::GetInstance()
{
	if (nullptr == m_instance) //if no instance
	{
		return TextureManager::CreateInstance(); //create instance
	}
	return m_instance; //return instance
}

void TextureManager::DestroyInstance()
{
	if (nullptr != m_instance) //if instance
	{
		delete m_instance; //delete instance
		m_instance = nullptr; //set to null
	}
}

TextureManager::TextureManager() : m_pTextureMap()
{}

TextureManager::~TextureManager()
{
	m_pTextureMap.clear();
}

//uses a std map as a texture directory and reference counting
unsigned int TextureManager::LoadTexture(const char* a_fileName)
{
	if (a_fileName != nullptr)
	{
		auto dictionaryIter = m_pTextureMap.find(a_fileName);
		if (dictionaryIter != m_pTextureMap.end())
		{
			//texture is already in map, increment ref and return text id
			TextureRef& textRef = (TextureRef&)(dictionaryIter->second);
			++textRef.refCount;
			return textRef.pTexture->GetTextureID();
		}
		else
		{
			//texture is not in dictionary load in from file
			Texture* pTexture = new Texture();
			if (pTexture->Load(a_fileName))
			{
				//successful load
				TextureRef textRef = { pTexture, 1 };
				m_pTextureMap[a_fileName] = textRef;
				return pTexture->GetTextureID();
			}
			else
			{
				delete pTexture;
				return 0;
			}
		}
	}
	return 0;
}

void TextureManager::ReleaseTexture(unsigned int a_texture)
{
	for (auto dictionaryIter = m_pTextureMap.begin(); dictionaryIter != m_pTextureMap.end(); ++dictionaryIter)
	{
		TextureRef& textRef = (TextureRef&)(dictionaryIter->second);
		if (a_texture == textRef.pTexture->GetTextureID())
		{
			//pre decrement will happen prior to call to ==
			if (--textRef.refCount == 0)
			{
				delete textRef.pTexture; //delete texture
				textRef.pTexture = nullptr; //set null
				m_pTextureMap.erase(dictionaryIter); //erase dictionary
				break;
			}
		}
	}
}

bool TextureManager::TextureExists(const char* a_fileName)
{
	auto dictIter = m_pTextureMap.find(a_fileName);
	return (dictIter != m_pTextureMap.end());
}

unsigned int TextureManager::GetTexture(const char* a_fileName) //returns texture ids from file if found
{
	auto dictIter = m_pTextureMap.find(a_fileName);
	if (dictIter != m_pTextureMap.end())
	{
		TextureRef& textRef = (TextureRef&)(dictIter->second);
		textRef.refCount++;
		return textRef.pTexture->GetTextureID();
	}
	return 0;
}