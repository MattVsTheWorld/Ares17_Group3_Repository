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
	Skybox();
	// A simple cubemap loading function taken from lab
	// Make more general for different levels!
	GLuint loadCubeMap(const char *fname[6], GLuint *texID);

	std::stack<glm::mat4> renderSkybox(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint meshObject, GLuint meshIndexCount);

protected:
private:
	GLuint skyboxProgram;
	GLuint textures[5];
};
#endif
