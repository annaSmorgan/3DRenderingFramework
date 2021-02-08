#include "includes/Application.h"
#include "includes/Utilities.h"
#include "includes/ShaderUtil.h"

//include OpenGl and GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//iostream for console logging
#include <iostream>
//this class deal with the core running of the application

void glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) //handles error messages
{
	std::string strSource = "0";
	std::string strType = strSource;
	std::string strSeverity = strSource;
	switch (source) {
	case GL_DEBUG_SOURCE_API: strSource = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: strSource = "WINDOWS"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: strSource = "SHADER COMP."; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: strSource = "3RD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION: strSource = "APP"; break;
	case GL_DEBUG_SOURCE_OTHER: strSource = "OTHER"; break;
	}
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: strType = "ERROR"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strType = "Deprecated"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: strType = "Undefined"; break;
	case GL_DEBUG_TYPE_PORTABILITY: strType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: strType = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER: strType = "Other"; break;
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: strSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: strSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW: strSeverity = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: strSeverity = "Notification"; break;
	}
	static int errNum = 0;

	std::cout << errNum << ": GL Error Callback: " << "\n Error Source: " << strSource << "\n Error Type: " << strType << "\n Severity: " << strSeverity << "\n Message: " << msg << std::endl;
	++errNum;
}

bool Application::Create(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullScreen) //sets up application window
{
	//Initialise GLFW
	if (!glfwInit())
	{
		return false;
	}
	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;
	//create a windowed mode window and it's OpenGL context 
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, a_applicationName,
		(a_fullScreen ? glfwGetPrimaryMonitor() : nullptr), nullptr);

	if (!m_window)
	{
		glfwTerminate(); 
		return false;
	}

	//make the window's context current 
	glfwMakeContextCurrent(m_window);

	//intialise GLAD - load in GL extens
	if (!gladLoadGL()) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}
	//get the supportef OpenGl version
	int major = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);
	int revision = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_REVISION);

	std::cout << "OpenGL Version " << major << "." << minor << "." << revision << std::endl;
	//implement a call to the derived class onCreate function for any implementation specific code
	bool result = onCreate();
	if (result == false)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	return result;
}

void Application::Run(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullScreen)//runs functions from child class myApplication and game loop
{
	if (Create(a_applicationName, a_windowWidth, a_windowHeight, a_fullScreen)) //if window created
	{
		Utility::resetTimer(); //reset timer
		m_running = true; //running application
		do
		{
			float deltaTime = Utility::tickTimer(); //sets up delta time

			Update(deltaTime); 

			Draw();

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		} while (m_running == true && glfwWindowShouldClose(m_window) == 0); //while running and window open

		Destroy();
	}

	ShaderUtil::DestroyInstance();
	//cleanup
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
