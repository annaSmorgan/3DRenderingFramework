#pragma once
//forward declare the GLFWindow structure
//avoid #include where possible
struct GLFWwindow;

//this class deal with the core running of the application
class Application
{
public: 
	//constructor, sets running to false
	Application() : m_window(nullptr), m_windowHeight(0), m_windowWidth(0), m_running(false) {}
	virtual ~Application() {}

	bool Create(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullScreen);
	void Run(const char* a_applicationName, unsigned int a_windowWidth, unsigned int a_windowHeight, bool a_fullScreen);
	void Quit() { m_running = false; }

protected:
	//pure vitual functions to be implemented by child classes
	virtual bool onCreate() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;

	//variables and references
	GLFWwindow* m_window;
	unsigned int m_windowWidth;
	unsigned int m_windowHeight;
	bool m_running;
};