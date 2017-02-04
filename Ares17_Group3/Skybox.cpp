#include "Skybox.h"

Skybox::Skybox(const char *cubeTexFiles[6]) {
	skyboxProgram = ShaderManager::initShaders("cubeMap.vert", "cubeMap.frag");
		loadCubeMap(cubeTexFiles, &textures);
}

GLuint Skybox::loadCubeMap(const char *fname[6], GLuint *texID) {
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0; i<6; i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}


void Skybox::renderSkybox(glm::mat4 projection, glm::mat4 view, Model *modelData) {
	// skybox as single cube using cube map

	std::stack<glm::mat4> mvStack;
	mvStack.push(view); // I can't make it use the matrix straight out
						// probably something stupid but ain't nobody got time
	glUseProgram(skyboxProgram);
	MeshManager::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	MeshManager::setUniformMatrix4fv(skyboxProgram, "view", glm::value_ptr(mvStack.top()));
	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures);
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	MeshManager::setUniformMatrix4fv(skyboxProgram, "modelMatrix", glm::value_ptr(model));
	modelData->Draw(skyboxProgram);
	
	mvStack.pop();
	mvStack.pop(); // :(
	glCullFace(GL_BACK); // drawing inside of cube!
						 // back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on

}

void Skybox::renderSkybox(glm::mat4 projection, glm::mat4 view, Model *modelData, GLuint texture) {
	// skybox as single cube using cube map

	std::stack<glm::mat4> mvStack;
	mvStack.push(view); // I can't make it use the matrix straight out
						// probably something stupid but ain't nobody got time
	glUseProgram(skyboxProgram);
	MeshManager::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	MeshManager::setUniformMatrix4fv(skyboxProgram, "view", glm::value_ptr(mvStack.top()));
	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	MeshManager::setUniformMatrix4fv(skyboxProgram, "modelMatrix", glm::value_ptr(model));
	modelData->Draw(skyboxProgram);

	mvStack.pop();
	mvStack.pop(); // :(
	glCullFace(GL_BACK); // drawing inside of cube!
						 // back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on

}

