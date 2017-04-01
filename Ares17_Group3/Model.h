#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

#define NUM_BONES_PER_VERTEX 4
class Model
{
public:
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path);
	// Draws the model, and thus all its meshes
	void Draw(GLuint shader);

	////////////////////////ANIMATION STUFF!!!!!/////////////////////////
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

	void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneInfo()
		{
			//initialise everything to zero
			//or is it:
			BoneOffset[0][0] = 1.0f; BoneOffset[0][1] = 0.0f; BoneOffset[0][2] = 0.0f; BoneOffset[0][3] = 0.0f;
			BoneOffset[1][0] = 0.0f; BoneOffset[1][1] = 1.0f; BoneOffset[1][2] = 0.0f; BoneOffset[1][3] = 0.0f;
			BoneOffset[2][0] = 0.0f; BoneOffset[2][1] = 0.0f; BoneOffset[2][2] = 1.0f; BoneOffset[2][3] = 0.0f;
			BoneOffset[3][0] = 0.0f; BoneOffset[3][1] = 0.0f; BoneOffset[3][2] = 0.0f; BoneOffset[3][3] = 1.0f;

			FinalTransformation[0][0] = 1.0f; FinalTransformation[0][1] = 0.0f; FinalTransformation[0][2] = 0.0f; FinalTransformation[0][3] = 0.0f;
			FinalTransformation[1][0] = 0.0f; FinalTransformation[1][1] = 1.0f; FinalTransformation[1][2] = 0.0f; FinalTransformation[1][3] = 0.0f;
			FinalTransformation[2][0] = 0.0f; FinalTransformation[2][1] = 0.0f; FinalTransformation[2][2] = 1.0f; FinalTransformation[2][3] = 0.0f;
			FinalTransformation[3][0] = 0.0f; FinalTransformation[3][1] = 0.0f; FinalTransformation[3][2] = 0.0f; FinalTransformation[3][3] = 1.0f;
			
			//for (int x = 0; x < NUM_BONES_PER_VERTEX; x++) {
			//	for (int y = 0; y < NUM_BONES_PER_VERTEX; y++) {
			//		BoneOffset[x][y] = 0.0f;
			//		FinalTransformation[x][y] = 0.0f;

			//	}
			//}
		}
	};

	static glm::mat4 AiToGLMMat4(aiMatrix4x4 in_mat)
	{
		glm::mat4 tmp;
		tmp[0][0] = in_mat.a1;
		tmp[1][0] = in_mat.b1;
		tmp[2][0] = in_mat.c1;
		tmp[3][0] = in_mat.d1;

		tmp[0][1] = in_mat.a2;
		tmp[1][1] = in_mat.b2;
		tmp[2][1] = in_mat.c2;
		tmp[3][1] = in_mat.d2;

		tmp[0][2] = in_mat.a3;
		tmp[1][2] = in_mat.b3;
		tmp[2][2] = in_mat.c3;
		tmp[3][2] = in_mat.d3;

		tmp[0][3] = in_mat.a4;
		tmp[1][3] = in_mat.b4;
		tmp[2][3] = in_mat.c4;
		tmp[3][3] = in_mat.d4;
		return tmp;
	}

	GLuint NumBones() const
	{
		return m_NumBones;
	}
	//	void Clear();

private:
	
	/*  Model Data  */
	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
	};

	map<string, GLuint> m_BoneMapping; // maps a bone name to its index
	GLuint m_NumBones;
	vector<BoneInfo> m_BoneInfo;

	vector<MeshEntry> m_Entry;
	glm::mat4 m_GlobalInverseTransform;
	
	/*  Model Data  */
	Assimp::Importer importer;
	const aiScene* scene;
	vector<Mesh> meshes;

	//std::vector<aiNode*> ai_nodes;
	//std::map<const aiNode*, std::pair<string, glm::mat4>> ai_nodes_names;
	//std::vector<aiNodeAnim*> ai_nodes_anim;
	//vector<Animations*> animations;

	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path);
	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Model::processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(GLuint MeshIndex, aiMesh* mesh);
	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	GLint TextureFromFile(const char* path, string directory);
};




