#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

//a basic vertex classs for an obj file, supports vertex pos, vetex normal, vertex uv coord
class OBJVertex
{
public:
	enum VertexAttributeFlags
	{
		POSITION	= (1 << 0), //pos of the vertex
		NORMAL		= (1 << 1), //normal of the vertex
		UVCOORD		= (1 << 2), //uv coords for the vertex
	};

	enum Offsets
	{
		PositionOffset	= 0,
		NormalOffset	= PositionOffset + sizeof(glm::vec4),
		UVCoordOffset	= NormalOffset + sizeof(glm::vec4),
	};

	OBJVertex();
	~OBJVertex();

	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 uvcoord;

	bool operator == (const OBJVertex& a_rhs) const;
	bool operator < (const OBJVertex& a_rhs) const;

};
//inline constructor destructor for OBJVertex class
inline OBJVertex::OBJVertex() : position(0, 0, 0, 1), normal(0, 0, 0, 0), uvcoord(0, 0) {}
inline OBJVertex::~OBJVertex() {}
//inline comparitor methods for objVertex
inline bool OBJVertex::operator == (const OBJVertex& a_rhs) const
{
	return memcmp(this, &a_rhs, sizeof(OBJVertex)) == 0;
}
inline bool OBJVertex::operator < (const OBJVertex& a_rhs) const
{
	return memcmp(this, &a_rhs, sizeof(OBJVertex)) < 0;
}

//an obj material
// materials have properties such as lights textures and roughness
class OBJMaterial
{
public:
	OBJMaterial() : name(), kA(0.0f), kD(0.0f), kS(0.0f) {};
	~OBJMaterial() {};

	std::string		name;
	//colours and illumination varaibles
	glm::vec4		kA; //ambient light colour - alpha component stores optcal density (Ni)(refraction index 0.001 - 10)
	glm::vec4		kD; //diffuse light colour - alpha component stores dissolve (d)(0-1)
	glm::vec4		kS; //specular light colour (exponent stored in alpha)

	//enum for the texture our obj model will support
	enum TextureTypes
	{
		DiffuseTexture = 0,
		SpecularTexture,
		NormalTexture,

		TextureTypes_Count
	};
	//texture will have filenames for loading, once loaded id's stored in id array
	std::string			textureFileNames[TextureTypes_Count];
	unsigned int		textureIDs[TextureTypes_Count];
};

//an obj model can be composed of many meshes. much like any 3D model
//lets use a class to store individual mesh data
class OBJMesh
{
public:
	OBJMesh();
	~OBJMesh();

	glm::vec4 calculateFaceNormal(const unsigned int& a_indexA, const unsigned int& a_indexB, const unsigned int& a_indexC) const;
	void calculateFaceNormals();

	std::string					m_name;
	std::vector<OBJVertex>		m_vertices;
	std::vector<unsigned int>	m_indicies;
	OBJMaterial*				m_material;
};
//inline constructor destructor -- to be expanded upon as required
inline OBJMesh::OBJMesh() {}
inline OBJMesh::~OBJMesh() {};

class OBJModel
{
public:
	OBJModel() : m_worldMatrix(glm::mat4(1.0f)), m_path(), m_meshes() {};
	~OBJModel()
	{
		unload(); // function to upload any data loaded in from file
	};
	//load from file function
	bool load(const char* a_fileName, float a_scale);
	//function to upload and free memory
	void unload();
	//functions to retrieve path, number of meshes and world matrix of model
	const char*			getPath()			const { return m_path.c_str(); }
	unsigned int		getMeshCount()		const { return m_meshes.size(); }
	const glm::mat4&	getWorldMatrix()	const { return m_worldMatrix; }
	unsigned int		getMaterialCount()	const { return m_materials.size(); }
	//functions to retrieve mesh by name or index for models that contains multiple meshes
	OBJMesh* getMeshByName(const char* a_name);
	OBJMesh* getMeshByIndex(unsigned int a_index);

	OBJMaterial* getMaterialByName(const char* a_name);
	OBJMaterial* getMaterialByIndex(unsigned int a_index);

	void controlMatrix(glm::vec3 m_translation);

private:
	//function to process line data read in file
	std::string lineType(const std::string& a_in);
	std::string lineData(const std::string& a_in);
	glm::vec4 processVectorString(const std::string a_data);
	std::vector<std::string> splitStringAtCharacter(std::string data, char a_character);

	void LoadMaterialLibrary(std::string a_mtllib);


	//obj face triplet struct
	typedef struct obj_face_triplet
	{
		unsigned int v;
		unsigned int vt;
		unsigned int vn;
	}obj_face_triplet;
	//function to exract triplet data from OBJ file
	obj_face_triplet ProcessTriplet(std::string a_triplet);

	//vector to store mesh data
	std::vector<OBJMesh*> m_meshes;
	//path to model data - useful for things like texture lookups
	std::string m_path;
	//root mat4 (world matrix)
	glm::mat4 m_worldMatrix;
	//vector stores materials
	std::vector<OBJMaterial*> m_materials;
};
