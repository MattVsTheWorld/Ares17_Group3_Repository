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
	HEALTH, ARMOR, AMMO
};

struct currentVals {
	double health;
	double armor;
	//double ammo;
};

class hudManager {
public:
	// functions provided in the labs
	GLuint textToTexture(const char * str, GLuint textID);
	void clearTextTexture(GLuint textID);
	hudManager(); // constructor, initializes values
	//void renderToHud(int value, GLuint shader, Model *modelData, glm::vec3 pos);
	void renderPlayerHud(std::string line, double value, valType _type, GLuint shader, Model *modelData, glm::vec3 pos, glm::vec3 color);
	void renderEditHud(std::string line, std::string value, GLuint shader, Model *modelData, glm::vec3 pos);

	void renderPause(GLuint shader, Model *modelData);
	void renderLoading(GLuint shader, Model *modelData);

private:
	TTF_Font * textFont;
	GLuint label;
	GLuint labelHP, labelArmor;
	GLuint pauseLabel;
	GLuint loadingScreen;
	currentVals _current;

	GLuint changeLabel(string line, double value) {
		std::string str = line;
		int val = value;
		str.append(std::to_string(val));
		const char *cstr = str.c_str();
		return textToTexture(cstr, label);
	}

};
#endif