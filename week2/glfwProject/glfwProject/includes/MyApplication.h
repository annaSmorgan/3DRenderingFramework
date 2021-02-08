#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class OBJModel;
//class deals with creating, drawing models and updating the models and application
class MyApplication : public Application
{
public:
	MyApplication();
	virtual ~MyApplication();

protected: //functions from application
	virtual bool onCreate();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Destroy();

private:
	//structure for a simple vertex - interleaved (positions, colour)
	typedef struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	}Vertex;

	typedef struct Line
	{
		Vertex v0;
		Vertex v1;
	}Line;
	//camera variables 
	glm::mat4 m_cameraMatrix;
	glm::mat4 m_projectionMatrix;

	//Shader programs
	unsigned int m_uiProgram;
	unsigned int m_objProgram;
	unsigned int m_lineVBO;
	unsigned int m_objModelBuffer[2];
	unsigned int m_vaoLines;
	unsigned int m_vaoOBJModel;

	//variables for light in scene
	glm::vec4 m_lightPos;
	glm::vec3 m_lightDiffuse;
	glm::vec3 m_lightAmbient;
	glm::vec3 m_lightSpecular;
	float SinMultiple = 0.5f;

	//model
	OBJModel* m_objModel;
	Line* lines;
	bool m_gridCheck = true; // acts as the toggle to determine whether grid is on or off
	const char* models[2]; //array of all the possible models
	//changable model var
	int modelPicker; //random number used to decide which model loads
	float modelSize = 0.05; //scale

	//skybox rendering
	unsigned int cubeMapTexID;
	unsigned int SBVAO;
	unsigned int SBVBO;
	unsigned int SBProgramID;

	//func
	void LightMovement(); //input and controls for the movement of the light source
	void CheckGrid(); //toggles the grid off and on
};