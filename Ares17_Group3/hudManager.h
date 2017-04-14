#ifndef HUDMANAGER
#define HUDMANAGER
#include <SDL_ttf.h>
#include "MeshManager.h"
#include "loadBitmap.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

enum valType {
	HEALTH, ARMOR//, AMMO
};

struct currentVals {
	double health;
	double armor;
	//double ammo;
};

class hudManager {
public:
	// functions provided in the labs
	GLuint textToTexture(const char * str, GLuint textID, TTF_Font* font);
	void clearTextTexture(GLuint textID);
	hudManager(bool shadows); // constructor, initializes values
	void renderPlayerHud(std::string line, double value, valType _type, GLuint shader, Model *modelData, glm::vec3 pos, glm::vec3 color);
	void renderEditHud(std::string line, std::string value, GLuint shader, Model *modelData, glm::vec3 pos);

	void renderCrosshair(GLuint shader, Model *modelData);

	void renderPause(GLuint shader, Model *modelData);
	void renderLoading(GLuint shader, Model *modelData);

	void renderMenu(GLuint shader, Model *modelData, bool shadows);

	void renderToScreen(GLuint shader, Model *modelData, GLuint label);
	void renderDefeat(GLuint shader, Model *modelData);
	void renderVictory(GLuint shader, Model *modelData);


	GLuint TextureFromFile(const char* path, string directory)
	{
		//Generate texture ID and load texture data 
		string filename = string(path);
		filename = directory + '/' + filename;
		GLuint textureID;
		glGenTextures(1, &textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
		return textureID;
	}

private:
	TTF_Font * textFont, * menuFont;
	GLuint label;
	GLuint labelHP, labelArmor;
	GLuint pauseLabel[3];
	GLuint loadingScreen;
	GLuint menuScreen;
	GLuint defeatScreen;
	GLuint victoryScreen;
	GLuint menuLabel[5];
	GLuint crosshair;
	currentVals _current;

	GLuint changeLabel(string line, double value) {
		std::string str = line;
		int val = static_cast<int>(value);
		str.append(std::to_string(val));
		const char *cstr = str.c_str();
		return textToTexture(cstr, label, textFont);
	}

};
#endif