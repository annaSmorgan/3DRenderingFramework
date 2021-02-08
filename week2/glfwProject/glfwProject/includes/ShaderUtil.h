#pragma once
#include <vector> 
//a utility class with handling shaders
class ShaderUtil
{
public:
	//handles instances
	static ShaderUtil* CreateInstance();
	static ShaderUtil* GetInstance();
	static void DestroyInstance();
	//handles shaders
	static unsigned int LoadShader(const char* a_fileName, unsigned int a_type);
	static void DeleteShader(unsigned int a_shaderID);
	//handles programs
	static unsigned int CreateProgram(const int& a_vertexShader, const int& a_fragmentShader);
	static void DeleteProgram(unsigned int a_program);

private:

	//private constructor and destructor
	//utilities implements a signleton design patttern
	ShaderUtil();
	~ShaderUtil();

	//vectors
	std::vector<unsigned int> mShaders;
	std::vector<unsigned int> mProgram;
	//handles interal shaders
	unsigned int LoadShadersInternal(const char* a_fileName, unsigned int a_type);
	void DeleteShaderInternal(unsigned int a_shaderID);
	//handles internal programs
	unsigned int CreateProgramInternal(const int& a_vertexShader, const int& a_fragmentShader);
	void DeleteProrgamInternal(unsigned int a_program);
	//reference
	static ShaderUtil* mInstance;
};




