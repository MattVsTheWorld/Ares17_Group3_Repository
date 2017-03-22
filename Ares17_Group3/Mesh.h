#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>
#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define LOCATION_VERTEX		0
#define LOCATION_COLOUR		1
#define LOCATION_NORMAL		2
#define LOCATION_TEXCOORD   3
#define LOCATION_INDEX		4
#define LOCATION_BONE_ID     5
#define LOCATION_BONE_WEIGHT 6



struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
	// Bone ID

	float weight[4];
	GLuint id[4];
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};



class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	/*  Functions  */
	// Constructor
	
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	//~Mesh();
	// Render the mesh
	void Draw(GLuint shader);

private:
	
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh();
};


