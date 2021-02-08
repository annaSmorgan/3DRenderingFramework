#include "includes\MyApplication.h"
#include "includes/ShaderUtil.h"
#include "includes/Utilities.h"
#include "includes/TextureManager.h"
#include "includes/Texture.h"

#include <objLoader.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <time.h>

//class deals with creating, drawing models and updating the models and application

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
}

bool MyApplication::onCreate()
{
	//get instance of tex man
	TextureManager::CreateInstance();
	//randomly selects either 0 or 1
	srand(time(NULL));
	modelPicker = rand() % 2;
	// sets the array to be full of the model locations
	models[0] = "resources/models/D0208009.obj";
	models[1] = "resources/models/C1102056.obj";

	//set the clear colour and enable depth testing and backface culling
	glClearColor(0.6f, 0.6f, 1.0f, 1.0f); //0.25 0.45 0.75 1.0
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//create shader program 
	unsigned int vertexShader = ShaderUtil::LoadShader("resources/shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = ShaderUtil::LoadShader("resources/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	m_uiProgram = ShaderUtil::CreateProgram(vertexShader, fragmentShader);

	//create a grid of lines to be drawn during update 
	lines = new Line[42];
	for (int i = 0, j = 0; i < 21; ++i, j += 2)
	{
		lines[j].v0.position = glm::vec4(-10 + i, 0.0f, 10.0f, 1.0f);
		lines[j].v0.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lines[j].v1.position = glm::vec4(-10 + i, 0.0f, -10.0f, 1.0f);
		lines[j].v1.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		lines[j + 1].v0.position = glm::vec4(10, 0.0f, -10.0f + i, 1.0f);
		lines[j + 1].v0.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lines[j + 1].v1.position = glm::vec4(-10, 0.0f, -10.0f + i, 1.0f);
		lines[j + 1].v1.colour = (i == 10) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	}

	//create a vertex buffer to hold our line data
	glGenBuffers(1, &m_lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	//fill vertex buffer with line data
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(Line) * 42, lines, 0);
	//generate vertex array object
	glGenVertexArrays(1, &m_vaoLines);
	glBindVertexArray(m_vaoLines);

	//enable the vertex array stae, since we're sending in an array of vertices
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//specify where our vertex array is, how many components each vertex has, 
	//the data type of each component and whether the data is normalised or not
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 16);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//create a world space matrix for a camera 
	m_cameraMatrix =
		glm::inverse(
			glm::lookAt(glm::vec3(10, 10, 10),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0))
		);
	//create a perspective projections matrix with a 90 degree field of view and widthscreen aspect ratio
	m_projectionMatrix =
		glm::perspective(
			glm::pi<float>() * 0.25f,
			m_windowWidth / (float)m_windowHeight,
			0.1f, 1000.0f);

	m_objModel = new OBJModel();

	if (m_objModel->load(models[modelPicker], modelSize)) // loads which ever random model was picked
	{
		TextureManager* pTM = TextureManager::GetInstance();
		//load in texture for model if any are present
		for (int i = 0; i < m_objModel->getMaterialCount(); ++i)
		{
			OBJMaterial* mat = m_objModel->getMaterialByIndex(i);
			for (int n = 0; n < OBJMaterial::TextureTypes::TextureTypes_Count; ++n)
			{
				if (mat->textureFileNames[n].size() > 0)
				{
					unsigned int textureID = pTM->LoadTexture(mat->textureFileNames[n].c_str());
					mat->textureIDs[n] = textureID;
				}
			}
		}

		//setup shader for obj model rendering
		//create obj shader program
		unsigned int obj_vertexShader = ShaderUtil::LoadShader("resources/shaders/obj_vertex.glsl", GL_VERTEX_SHADER);
		unsigned int obj_fragmentShader = ShaderUtil::LoadShader("resources/shaders/obj_fragment.glsl", GL_FRAGMENT_SHADER);
		m_objProgram = ShaderUtil::CreateProgram(obj_vertexShader, obj_fragmentShader);
		//set up vertex and index buffer for OBJ rendering
		glGenBuffers(2, m_objModelBuffer);

		glGenVertexArrays(1, &m_vaoOBJModel);
		glBindVertexArray(m_vaoOBJModel);

		//set up vertex buffer date
		glBindBuffer(GL_ARRAY_BUFFER, m_objModelBuffer[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objModelBuffer[1]);
		glEnableVertexAttribArray(0);	//position
		glEnableVertexAttribArray(1);	//normal
		glEnableVertexAttribArray(2);	//uv coord

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + OBJVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), ((char*)0) + OBJVertex::NormalOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), ((char*)0) + OBJVertex::UVCoordOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else
	{
		std::cout << "Failed to load model" << std::endl;
		return false;
	}

	//Loading in the skybox
	std::vector<std::string> texture_faces = { "resources/skybox/right.jpg", "resources/skybox/left.jpg",
											  "resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
											  "resources/skybox/front.jpg", "resources/skybox/back.jpg" };
	//cube map texture set up
	unsigned int cubemap_image_tag[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
	//reference to texture class
	Texture* pTexture = new Texture();
	cubeMapTexID = pTexture->LoadCubeMap(texture_faces, cubemap_image_tag);
	//make VBO and AVO and load shaders for skybox
	unsigned int SB_vertexShader = ShaderUtil::LoadShader("resources/shaders/SB_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int SB_fragmentShader = ShaderUtil::LoadShader("resources/shaders/SB_fragment.glsl", GL_FRAGMENT_SHADER);
	SBProgramID = ShaderUtil::CreateProgram(SB_vertexShader, SB_fragmentShader);
	//vertices layout for skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	//create a vertex buffer to hold our line data
	glGenBuffers(1, &SBVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SBVBO);
	//fill vertex buffer with line data
	glBufferStorage(GL_ARRAY_BUFFER, 108 * sizeof(float), skyboxVertices, 0);
	//generate vertex array object
	glGenVertexArrays(1, &SBVAO);
	glBindVertexArray(SBVAO);

	//as we have sent the line data to the gpu we no longer require it on the cpu side memory
	//enable the vertex array stae, since we're sending in an array of vertices
	glEnableVertexAttribArray(0);

	//specify where our vertex array is, how many components each vertex has, 
	//the data type of each component and whether the data is normalised or not
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glBindBuffer(GL_ARRAY_BUFFER, SBVBO);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//setting up the light
	m_lightPos = glm::vec4(20.0f, 15.0f, 0.0f, 1.0f); 
	m_lightAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
	m_lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

	//controls displayed to console
	std::cout << "\n" << std::endl;
	std::cout << "====================" << std::endl;
	std::cout << "CONTROLS:" << std::endl;
	std::cout << "x - GRID OFF" << std::endl;
	std::cout << "Z - GRID ON" << std::endl;
	std::cout << "WASD - MOVE" << std::endl;
	std::cout << "Q/E - VERTICAL DISPLACMENT" << std::endl;
	std::cout << "SHIFT - SPEED UP" << std::endl;
	std::cout << "C - CLOCKWISE MOTION ON LIGHT" << std::endl;
	std::cout << "V - ANTICLOCKWISE MOTION ON LIGHT" << std::endl;
	std::cout << "B - STOPS MOTION ON LIGHT" << std::endl;
	std::cout << "====================" << std::endl;

	return true;
}

void MyApplication::Update(float deltaTime)
{
	Utility::FreeMovement(m_cameraMatrix, deltaTime, 4.0f); //run the func for camera movement
	//Utility::MatrixMovement();
	
	CheckGrid(); //check for grid inputs 
	LightMovement(); //check for light inputs

	//calc and move the light source in a circular motion
	float S = sinf(deltaTime * SinMultiple);
	float C = cosf(deltaTime * 0.5f);
	glm::vec3 temp = m_lightPos;
	m_lightPos.x = temp.x * C - temp.z * S;
	m_lightPos.z = temp.x * S + temp.z * C;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//get current window context
	GLFWwindow* window = glfwGetCurrentContext();
	//quit application when esc is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Quit();
	}

}

void MyApplication::Draw()
{
	//clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	//get the view matrix from the world space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	glm::mat4 projectionViewMatrix = m_projectionMatrix * viewMatrix;

	//enable shaders
	glUseProgram(m_uiProgram);

	glBindVertexArray(m_vaoLines);
	   
	//send the projection matrix to the vertex shader
	//ask the shader program for the location of the projection view matrix uniform variable
	unsigned int projectionViewUniformLocation = glGetUniformLocation(m_uiProgram, "ProjectionViewMatrix");
	//send this location a pointer to our glm::mat4 
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(projectionViewMatrix));
	//if grid check is true draw the grid
	if (m_gridCheck)
	{
		glDrawArrays(GL_LINES, 0, 42 * 2);
	}
	glBindVertexArray(0);
	glUseProgram(0);

	glUseProgram(m_objProgram);
	glBindVertexArray(m_vaoOBJModel);
	//set the projection view matrix for this shader
	projectionViewUniformLocation = glGetUniformLocation(m_objProgram, "ProjectionViewMatrix");
	//send this location a pointer to our glm::mat4 (send across float data)
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(projectionViewMatrix));
	
	//send across light data
	glm::vec4 lightDir = glm::normalize(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - m_lightPos);
	int LightDirectionUniform = glGetUniformLocation(m_objProgram, "LightDir");
	glUniform4fv(LightDirectionUniform, 1, glm::value_ptr(lightDir));

	int LightAmbientUniform = glGetUniformLocation(m_objProgram, "LightAmbient");
	glUniform3fv(LightAmbientUniform, 1, glm::value_ptr(m_lightAmbient));

	int LightDiffuseUniform = glGetUniformLocation(m_objProgram, "LightDiffuse");
	glUniform3fv(LightDiffuseUniform, 1, glm::value_ptr(m_lightDiffuse));

	int LightSpecularUniform = glGetUniformLocation(m_objProgram, "LightSpecular");
	glUniform3fv(LightSpecularUniform, 1, glm::value_ptr(m_lightSpecular));

	for (int i = 0; i < m_objModel->getMeshCount(); ++i)
	{
		//get the model matrix location from the shader program
		int modelMatrixUniformLocation = glGetUniformLocation(m_objProgram, "ModelMatrix");
		//send the obj model's world matrix data across to the shader program
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, false, glm::value_ptr(m_objModel->getWorldMatrix()));

		int cameraPositionUniformLocation = glGetUniformLocation(m_objProgram, "camPos");
		glUniform4fv(cameraPositionUniformLocation, 1, glm::value_ptr(m_cameraMatrix[3]));

		OBJMesh* pMesh = m_objModel->getMeshByIndex(i);
		//send material data to shader
		int kA_location = glGetUniformLocation(m_objProgram, "kA");
		int kD_location = glGetUniformLocation(m_objProgram, "kD");
		int kS_location = glGetUniformLocation(m_objProgram, "kS");

		OBJMaterial* pMaterial = pMesh->m_material;
		if (pMaterial != nullptr)
		{
			//send the obj model's world matrix data across to the shader program
			glUniform4fv(kA_location, 1, glm::value_ptr(pMaterial->kA));
			glUniform4fv(kD_location, 1, glm::value_ptr(pMaterial->kD));
			glUniform4fv(kS_location, 1, glm::value_ptr(pMaterial->kS));

			//get the location of the diffuse tetxure
			int textUniformLoc = glGetUniformLocation(m_objProgram, "DiffuseTexture");
			glUniform1i(textUniformLoc, 0); //set diffuse texture to be gl_texture0

			glActiveTexture(GL_TEXTURE0); //set the active texture unit to text 0
			//bind the texture for diffuse for this material to the tex 0
			glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::DiffuseTexture]);

			//get the location of the specular tetxure
			textUniformLoc = glGetUniformLocation(m_objProgram, "SpecularTexture");
			glUniform1i(textUniformLoc, 1); //set diffuse texture to be gl_texture1

			glActiveTexture(GL_TEXTURE1); //set the active texture unit to text 1
			//bind the texture for diffuse for this material to the tex 1
			glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::SpecularTexture]);

			//get the location of the normal tetxure
			textUniformLoc = glGetUniformLocation(m_objProgram, "NornalTexture");
			glUniform1i(textUniformLoc, 2); //set diffuse texture to be gl_texture2

			glActiveTexture(GL_TEXTURE2); //set the active texture unit to text 2
			//bind the texture for diffuse for this material to the tex 2
			glBindTexture(GL_TEXTURE_2D, pMaterial->textureIDs[OBJMaterial::TextureTypes::NormalTexture]);

		}
		else //no material to obtain lighting info from use defaults
		{
			//send the obj model's world matrix data across to the shader program
			glUniform4fv(kA_location, 1, glm::value_ptr(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)));
			glUniform4fv(kD_location, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
			glUniform4fv(kS_location, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 64.0f)));
		}

		//draw
		glBindBuffer(GL_ARRAY_BUFFER, m_objModelBuffer[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objModelBuffer[1]);

		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(OBJVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indicies.size() * sizeof(unsigned int), pMesh->m_indicies.data(), GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, pMesh->m_indicies.size(), GL_UNSIGNED_INT, 0);

	}
	
	glBindVertexArray(0);

	glUseProgram(0);

	//draw skybox
	glDepthFunc(GL_LEQUAL);
	glUseProgram(SBProgramID);
	glDepthMask(GL_FALSE);

	//set the projection view matrix for this shader
	projectionViewUniformLocation = glGetUniformLocation(SBProgramID, "ProjectionViewMatrix");
	//send this location a pointer to our glm::mat4 (send across float data)
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(projectionViewMatrix));
	glBindVertexArray(SBVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glUseProgram(0);

 }

void MyApplication::Destroy() //destory 
{
	delete m_objModel;
	delete[] lines;
	glDeleteBuffers(1, &m_lineVBO);
	glDeleteBuffers(1, m_objModelBuffer);
	glDeleteVertexArrays(1, &m_vaoLines);
	glDeleteVertexArrays(1, &m_vaoOBJModel);
	ShaderUtil::DeleteProgram(m_uiProgram);
	TextureManager::DestroyInstance();
	ShaderUtil::DestroyInstance();
	
}

void MyApplication::CheckGrid()
{
	//get current window context
	GLFWwindow* window = glfwGetCurrentContext();
	//check if Z was pressed then set the grid toggle check to true
	if (glfwGetKey(window, 'Z') == GLFW_PRESS)
	{
		m_gridCheck = true;
	}
	//if X was pressed then set the grid toggle check to false
	if (glfwGetKey(window, 'X') == GLFW_PRESS)
	{
		m_gridCheck = false;
	}
}

void MyApplication::LightMovement()
{
	//get current window context
	GLFWwindow* window = glfwGetCurrentContext();
	//check if C was pressed speeds up light default direction
	if (glfwGetKey(window, 'C') == GLFW_PRESS)
	{
		SinMultiple += 0.1f;
	}
	//if V was pressed slows down or reverse light default direction
	if (glfwGetKey(window, 'V') == GLFW_PRESS)
	{
		SinMultiple -= 0.1f;
	}
	//if B is pressed stop the light
	if (glfwGetKey(window, 'B') == GLFW_PRESS)
	{
		SinMultiple = 0.0f;
	}
}


