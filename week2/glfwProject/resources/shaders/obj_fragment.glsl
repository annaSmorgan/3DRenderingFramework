#version 400 

smooth in vec2 vertUV; 
smooth in vec4 vertNormal;
smooth in vec4 vertPos;

out vec4 outputColour; 

uniform vec4 camPos;

uniform vec4 kA;
uniform vec4 kD;
uniform vec4 kS;

//uniform for texture data
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;

uniform vec4 LightDir;

void main()
{
	//get texture data from uv coords
	vec3 DiffuseTextureData = texture(DiffuseTexture, vertUV).rgb;
	vec3 NormalTextureData = texture(NormalTexture, vertUV).rgb;
	vec3 SpecularTextureData = texture(SpecularTexture, vertUV).rgb;

	vec3 Ambient = kA.xyz * LightAmbient; //ambient light

	//get lambertain term
	float nDL = max(0.0f, dot(normalize(vertNormal), -LightDir));
	vec3 Diffuse = kD.xyz * LightDiffuse * nDL * DiffuseTextureData.rgb;

	vec3 R = reflect(LightDir, normalize(vertNormal)).xyz; //reflected light vector
	vec3 E = normalize(camPos - vertPos).xyz; //surface to eye vector

	float specTerm = pow(max(0.0f, dot(E, R)), kS.a); //specular term
	vec3 Specular = LightSpecular * kS.xyz * SpecularTextureData.rgb * specTerm;

	outputColour = vec4(Ambient + Diffuse + Specular, 1.0f);
	
}