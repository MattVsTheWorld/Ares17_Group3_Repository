#ifndef SKYBOX
#define SKYBOX
#include "MeshManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL.h"
#include <stack>
#include <iostream>

class Skybox {
public:
	Skybox(const char *cubeTexFiles[6]);
	// A simple cubemap loading function taken from lab
	// Make more general for different levels!
	GLuint loadCubeMap(const char *fname[6], GLuint *texID);

	void renderSkybox(glm::mat4 projection, glm::mat4 view, Model* modelData);
	void renderSkybox(glm::mat4 projection, glm::mat4 view, Model *modelData, GLuint texture);

protected:
private:
	GLuint skyboxProgram;
	GLuint textures;
};
#endif
