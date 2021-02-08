#include "includes/MyApplication.h"

//main class which starts the running of application
int main()
{
	MyApplication* myApp = new MyApplication(); 
	myApp->Run("Rendering Framework", 1280, 720, false); //runs application start up by setting its name and dimensions
	delete myApp;
	return 0;
}

