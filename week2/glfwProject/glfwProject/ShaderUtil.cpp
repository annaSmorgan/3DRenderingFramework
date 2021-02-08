#include "includes/ShaderUtil.h"
#include "includes/Utilities.h"

#include <glad/glad.h>
#include <iostream>
//a utility class with handling shaders

//static instance of utilities
ShaderUtil* ShaderUtil::mInstance = nullptr;
//singleton creation, fetch and destory functionality
ShaderUtil* ShaderUtil::GetInstance()
{
	if (mInstance == nullptr) //if no instance create one
	{
		return ShaderUtil::CreateInstance();
	}
	return mInstance; //return instance
}

ShaderUtil* ShaderUtil::CreateInstance()
{
	if (mInstance == nullptr) //if no instance create one
	{
		mInstance = new ShaderUtil();
	}
	else //if there is send error message
	{
		//print to console that attempt to create multiple instances of utilities
		std::cout << "Attempt to create multiple instances of Utilities" << std::endl;
	}
	return mInstance; //return instance
}

void ShaderUtil::DestroyInstance()
{
	if (mInstance != nullptr) //if instance
	{
		delete mInstance; //delete it
		mInstance = nullptr; //set to null
	}
	else //if not send error message
	{
		//print to console that attempt to destory null instance of utilites
		std::cout << "Attmept to destory nullinstance of Utilities" << std::endl;
	}
}
ShaderUtil::ShaderUtil()
{}

ShaderUtil::~ShaderUtil()
{
	//delete any shaders that have not been unloaded 
	for (auto iter = mShaders.begin(); iter != mShaders.end(); ++iter)
	{
		glDeleteShader(*iter);
	}
	//destroy any programs that are still about
	for (auto iter = mProgram.begin(); iter != mProgram.end(); ++iter)
	{
		glDeleteProgram(*iter);
	}
}

unsigned int ShaderUtil::LoadShader(const char* a_fileName, unsigned int a_type)
{
	ShaderUtil* instance = ShaderUtil::GetInstance(); //create instance
	return instance->LoadShadersInternal(a_fileName, a_type); 
}

unsigned int ShaderUtil::LoadShadersInternal(const char* a_fileName, unsigned int a_type)
{
	//int to test for shader creation success
	int success = GL_FALSE;
	//grab the shader course from the file
	char* source = Utility::FileToBuffer(a_fileName);
	unsigned int shader = glCreateShader(a_type);
	//set the source buffer for the shader
	glShaderSource(shader, 1, (const char**)&source, 0);
	glCompileShader(shader);
	//as the buffer from fileToBuffer was allocated this needs to be destoryed
	delete[] source;

	//test shader compliation for any errors and display them to console
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (GL_FALSE == success) //shader compilation failed, get logs and display them to console
	{
		int infoLogLength = 0; //variable to store the length of the error log
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength]; //allocate buffer to hold data
		glGetShaderInfoLog(shader, infoLogLength, 0, infoLog);
		std::cout << "Unable to compile: " << a_fileName << std::endl;
		std::cout << infoLog << std::endl;
		delete[] infoLog;
		return 0;
	}
	//success - add shader to mShader vector
	mShaders.push_back(shader);
	return shader;
}

void ShaderUtil::DeleteShader(unsigned int a_shader)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	instance->DeleteShaderInternal(a_shader);
}

void ShaderUtil::DeleteShaderInternal(unsigned int a_shader)
{
	//loop through the shaders vector
	for (auto iter = mShaders.begin(); iter != mShaders.end(); ++iter)
	{
		if (*iter == a_shader) //if we find the shader we are looking for
		{
			glDeleteShader(*iter); // delete shader
			mShaders.erase(iter); //remove this item from the shaders vector
			break;
		}
	}
}
unsigned int ShaderUtil::CreateProgram(const int& a_vertexShader, const int& a_fragmentShader)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	return instance->CreateProgramInternal(a_vertexShader, a_fragmentShader);
}

unsigned int ShaderUtil::CreateProgramInternal(const int& a_vertexShader, const int& a_fragmentShader)
{
	//boolean value to test for shader program linkage success
	int success = GL_FALSE;

	//create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, a_vertexShader);
	glAttachShader(handle, a_fragmentShader);
	//link the shaders together into one shader program
	glLinkProgram(handle);
	//test to see if the program was successfully created
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (GL_FALSE == success) //if something has gone wrong then execute the following
	{
		int infoLogLength = 0; //int value to tell us the length of the error log
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		//allocate enough space in a buffer for the error message
		char* infoLog = new char[infoLogLength];
		//fill the buffer with data
		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		//prnt log message to console
		std::cout << "Shader Linker Error" << std::endl;
		std::cout << infoLog << std::endl;

		//delete the char buffer now we have displayed it
		delete[] infoLog;
		return 0; //return 0, programID 0 is null program
	}
	//add the porgram to the shader program vector
	mProgram.push_back(handle);
	return handle; //return the program ID
}

void ShaderUtil::DeleteProgram(unsigned int a_program)
{
	ShaderUtil* instance = ShaderUtil::GetInstance();
	instance->DeleteProrgamInternal(a_program);
}

void ShaderUtil::DeleteProrgamInternal(unsigned int a_program)
{
	//loop through the program vector
	for (auto iter = mProgram.begin(); iter != mProgram.end(); ++iter)
	{
		if (*iter == a_program) //if we find the program we are looking for
		{
			glDeleteProgram(*iter); // delete program
			mProgram.erase(iter); //remove this item from the prorgam vector
			break;
		}
	}
}
