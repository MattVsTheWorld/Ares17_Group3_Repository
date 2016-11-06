#include "hudManager.h"

// functions provided in the labs
// allow to render text to a texture
GLuint hudManager::textToTexture(const char * str, GLuint textID) {
	GLuint texture = textID;
	TTF_Font * font = textFont;

	SDL_Surface * stringImage = TTF_RenderText_Blended(font, str, { 255, 255, 255 });

	if (stringImage == NULL) {
		std::cout << "String surface not created." << std::endl;
	}

	if (texture == 0) {
		glGenTextures(1, &texture);//This avoids memory leakage, only initialise //first time
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);
	return texture;
}
void hudManager::clearTextTexture(GLuint textID) {
	if (textID != NULL) {
		glDeleteTextures(1, &textID);
	}
}

// initialize hud manager, open font
hudManager::hudManager() {
	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		std::cout << "TTF failed to initialise." << std::endl;

	textFont = TTF_OpenFont("MavenPro-Regular.ttf", 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;

	// initialization of variables
	label = 0;

}



// render normal hud text
// uses shader program, light, modelview top of stack, mesh object and index count as parameters
void hudManager::renderFPS(GLuint shader, MeshManager::lightStruct light, glm::mat4 top, GLuint meshObject, GLuint meshIndexCount, float fps) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	std::string str = "FPS: ";
	const char *cstr = str.c_str();
	str.append(std::to_string(fps)); // render fps

	glUseProgram(shader); //texture-only shader will be used for teture rendering
	MeshManager::setLight(shader, light);
	label = textToTexture(cstr, label);
	glBindTexture(GL_TEXTURE_2D, label);
	// transformations
	top = glm::translate(top, glm::vec3(-0.85f, 0.9f, 0.9f));
	top = glm::scale(top, glm::vec3(0.125f, 0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(top));

	MeshManager::drawIndexedMesh(meshObject, meshIndexCount, GL_TRIANGLES);
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 
}