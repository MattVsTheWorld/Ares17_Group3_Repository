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
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
	void LoadBones(GLuint MeshIndex, const aiMesh* paiMesh);

	void BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms);

	struct BoneInfo
	{
		aiMatrix4x4 BoneOffset;
		aiMatrix4x4 FinalTransformation;

		BoneInfo()
		{
			//initialise everything to zero
			for (int x = 0; x < NUM_BONES_PER_VERTEX; x++) {
				for (int y = 0; y < NUM_BONES_PER_VERTEX; y++) {
					BoneOffset[x][y] = 0.0f;
					FinalTransformation[x][y] = 0.0f;

				}
			}
		}
	};

	GLuint NumBones() const
	{
		return m_NumBones;
	}
	//	void Clear();

	struct VertexBoneData
	{
		GLuint IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			//initialise everything to zero
			for (int i = 0; i < NUM_BONES_PER_VERTEX; i++) {
				IDs[i] = 0;
				Weights[i] = 0.0f;
			}
		}

		void AddBoneData(GLuint BoneID, float Weight);
	};


private:
	
	/*  Model Data  */
	struct MeshEntry {
		MeshEntry()
		{
			BaseVertex = 0;
		}

		unsigned int BaseVertex;
	};

	vector<VertexBoneData> BoneData;

	map<string, GLuint> m_BoneMapping; // maps a bone name to its index
	GLuint m_NumBones;
	vector<BoneInfo> m_BoneInfo;

	vector<MeshEntry> m_Entry;
	aiMatrix4x4 m_GlobalInverseTransform;
	
	/*  Model Data  */
	const aiScene* scene;
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path);
	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void Model::processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(GLuint MeshIndex, aiMesh* mesh, const aiScene* scene);
	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	GLint TextureFromFile(const char* path, string directory);
};




