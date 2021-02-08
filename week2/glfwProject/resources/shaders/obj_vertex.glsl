#version 400 

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal; 
layout(location = 2) in vec2 uvCoord;

smooth out vec4 vertPos;
smooth out vec2 vertUV;
smooth out vec4 vertNormal;

uniform mat4 ProjectionViewMatrix;
uniform mat4 ModelMatrix;

void main() 
{ 
	vertUV = uvCoord;
	vertNormal = normal;
	vertPos = ModelMatrix * position; // world space postion
	gl_Position = ProjectionViewMatrix * ModelMatrix * position; //screen space position
}