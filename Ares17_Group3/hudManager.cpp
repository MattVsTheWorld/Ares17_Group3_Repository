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


// Requires change ***

// render normal hud text
// uses shader program, light, modelview top of stack, mesh object and index count as parameters
/*void hudManager::renderToHud(int value, GLuint shader, Model *modelData, glm::vec3 pos) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	std::string str = "FPS: ";	
	str.append(std::to_string(value));	

	const char *cstr = str.c_str();
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	label = textToTexture(cstr, label);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, label);
	// transformations
	id = glm::translate(id, pos);
	id = glm::scale(id, glm::vec3(0.075f, -0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}*/


void hudManager::renderPlayerHud(std::string line, double value, GLuint shader, Model *modelData, glm::vec3 pos, glm::vec3 color) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	std::string str = line;
	int val = value;
	str.append(std::to_string(val));
	const char *cstr = str.c_str();
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	label = textToTexture(cstr, label);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, label);
	// transformations
	id = glm::translate(id, pos);
	id = glm::scale(id, glm::vec3(0.1f, -0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	//MeshManager::setUniformMatrix4fv(shader, "text_color", glm::value_ptr(color));
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(color));
	
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void hudManager::renderEditHud(std::string line, std::string value, GLuint shader, Model *modelData, glm::vec3 pos) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	std::string str = line.append(": ");
	str.append(value);

	const char *cstr = str.c_str();
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	label = textToTexture(cstr, label);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, label);
	// transformations
	id = glm::translate(id, pos);
	id = glm::scale(id, glm::vec3(0.1f, -0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(glm::mat4(1.0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}