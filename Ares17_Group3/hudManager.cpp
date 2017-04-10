#include "hudManager.h"

// functions provided in the labs
// allow to render text to a texture
GLuint hudManager::textToTexture(const char * str, GLuint textID, TTF_Font* font) {
	GLuint texture = textID;

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

	//cout << "buh why";

	SDL_FreeSurface(stringImage);
	return texture;
}
void hudManager::clearTextTexture(GLuint textID) {
	if (textID != NULL) {
		glDeleteTextures(1, &textID);
	}
}

// initialize hud manager, open font
hudManager::hudManager(bool shadows) {
	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		std::cout << "TTF failed to initialise." << std::endl;

	textFont = TTF_OpenFont("../Ares17_Group3/Fonts/Capture_it.ttf", 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;

	menuFont = TTF_OpenFont("../Ares17_Group3/Fonts/Roboto-Black.ttf", 32);
	if (menuFont == NULL)
		std::cout << "Failed to open font." << std::endl;

	// initialization of variables
	label = 0;


	std::string str = "Game paused";
	const char *cstr = str.c_str();
	pauseLabel[0] = textToTexture(cstr, pauseLabel[0], textFont);
	
	str = "[P] Resume";
	cstr = str.c_str();
	pauseLabel[1] = textToTexture(cstr, pauseLabel[1], textFont);

	str = "[Esc] Back to menu";
	cstr = str.c_str();
	pauseLabel[2] = textToTexture(cstr, pauseLabel[2], textFont);

	str = "Ares 17";
	cstr = str.c_str();
	menuLabel[0] = textToTexture(cstr, menuLabel[0], menuFont);
	
	str = "[1] Play";
	cstr = str.c_str();
	menuLabel[1] = textToTexture(cstr, menuLabel[1], menuFont);

	str = "[2] Restart";
	cstr = str.c_str();
	menuLabel[2] = textToTexture(cstr, menuLabel[2], menuFont);

	str = "[3] Shadows - ";
	str.append(shadows ? "ON" : "OFF");
	cstr = str.c_str();
	menuLabel[3] = textToTexture(cstr, menuLabel[3], menuFont);

	str = "[6] Quit";
	cstr = str.c_str();
	menuLabel[4] = textToTexture(cstr, menuLabel[4], menuFont);

	crosshair = TextureFromFile("crosshair.png", "../Ares17_Group3/Textures");
	// https://dribbble.com/shots/897447-Ares-Logo-Mark
	menuScreen = loadBitmap::loadBitmap("../Ares17_Group3/Textures/menu.bmp");
	loadingScreen = loadBitmap::loadBitmap("../Ares17_Group3/Textures/loading.bmp");
	defeatScreen = loadBitmap::loadBitmap("../Ares17_Group3/Textures/defeat.bmp");
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


void hudManager::renderPlayerHud(std::string line, double value, valType _type, GLuint shader, Model *modelData, glm::vec3 pos, glm::vec3 color) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	
	if (_type == HEALTH && _current.health != value){
		labelHP = changeLabel(line, value);
		_current.health = value;
	}
	if ((_type == ARMOR && _current.armor != value)) {
		labelArmor = changeLabel(line, value);
		_current.armor = value;
	}
	/*std::string str = line;
	int val = value;
	str.append(std::to_string(val));
	const char *cstr = str.c_str();*/
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	//label = textToTexture(cstr, label);
	glActiveTexture(GL_TEXTURE0);
	if(_type == HEALTH)
		glBindTexture(GL_TEXTURE_2D, labelHP);
	if (_type == ARMOR)
		glBindTexture(GL_TEXTURE_2D, labelArmor);
	// transformations
	id = glm::translate(id, pos);
	id = glm::scale(id, glm::vec3(0.1f, -0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(color));
	modelData->Draw(shader);
	clearTextTexture(label); // ?
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, 0);
}


void hudManager::renderEditHud(std::string line, std::string value, GLuint shader, Model *modelData, glm::vec3 pos) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	std::string str = line.append(": ");
	str.append(value);

	const char *cstr = str.c_str();
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	label = textToTexture(cstr, label, textFont);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, label);
	// transformations
	id = glm::translate(id, pos);
	id = glm::scale(id, glm::vec3(0.1f, -0.075f, 0.075f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void hudManager::renderCrosshair(GLuint shader, Model *modelData) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
						  // transformations
	
	id = glm::translate(id, glm::vec3(0.075, -0.1, 0.0));
	id = glm::scale(id, glm::vec3(0.0325f, -0.05f, 1.0f));
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(glm::vec3(0, 0, 0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crosshair);
	modelData->Draw(shader);


	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}



void hudManager::renderPause(GLuint shader, Model *modelData) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	// transformations
	id = glm::translate(id, glm::vec3(0.0,0.5,0.0));
	id = glm::scale(id, glm::vec3(0.4f, -0.25f, 1.0f));
	//I though this was purple but good enough ¯\_(?)_/¯
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(glm::vec3(0.183, 0.045, 0.087))); 
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pauseLabel[0]);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, 1.5, 0.0));
	id = glm::scale(id, glm::vec3(0.5f, 0.5f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pauseLabel[1]);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, 2.0, 0.0));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pauseLabel[2]);
	modelData->Draw(shader);

	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void hudManager::renderLoading(GLuint shader, Model *modelData) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
	// transformations
	id = glm::scale(id, glm::vec3(1.0f, -1.0f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadingScreen);
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void hudManager::renderMenu(GLuint shader, Model *modelData, bool shadows) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
						  // transformations
	id = glm::scale(id, glm::vec3(1.0f, -1.0f, 1.0f));
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(glm::vec3(0.0,0.0,0.0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuScreen);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, -0.65, 0.0));
	id = glm::scale(id, glm::vec3(0.2f, 0.1f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuLabel[0]);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, 3.0, 0.0));
	id = glm::scale(id, glm::vec3(0.6f, 0.8f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuLabel[1]);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, 2.5, 0.0));
	id = glm::scale(id, glm::vec3(1.3f, 1.0f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuLabel[2]);
	modelData->Draw(shader);

	
	string str = "[3] Shadows - ";
	str.append(shadows ? "ON" : "OFF");
	const char* cstr = str.c_str();
	menuLabel[3] = textToTexture(cstr, menuLabel[3], menuFont);
	
	id = glm::translate(id, glm::vec3(0.0, 2.5, 0.0));
	id = glm::scale(id, glm::vec3(1.2f, 1.0f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuLabel[3]);
	modelData->Draw(shader);

	id = glm::translate(id, glm::vec3(0.0, 2.5, 0.0));
	//id = glm::scale(id, glm::vec3(1.7f, 1.0f, 1.0f));
	id = glm::scale(id, glm::vec3(0.5f, 1.0f, 1.0f));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, menuLabel[4]);
	modelData->Draw(shader);

	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void hudManager::renderDefeat(GLuint shader, Model *modelData) {
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	glm::mat4 id = glm::mat4();
	glUseProgram(shader); //texture-only shader will be used for teture rendering
						  // transformations
	id = glm::scale(id, glm::vec3(1.0f, -1.0f, 1.0f));
	glUniform3fv(glGetUniformLocation(shader, "text_color"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	MeshManager::setUniformMatrix4fv(shader, "model", glm::value_ptr(id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defeatScreen);
	modelData->Draw(shader);
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
