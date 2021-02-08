#pragma once

#include <glm/glm.hpp>

//a utility class with static helper methods
class Utility
{
public:
	//utilities for timing;; Get() updates timer and returns time since lasy get call
	static void  resetTimer();
	static float tickTimer();
	static float getDeltaTime();
	static float getTotalTime();

	//helper function for loading shader code into memory
	static char* FileToBuffer(const char* a_szPath);

	//utility for mouse / keyboard movement of a matrix transform (suitable for camera)
	static void FreeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up = glm::vec3(0, 1, 0));

	//utlity for the control of the world matrix
	//static void MatrixMovement();


};