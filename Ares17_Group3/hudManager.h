#ifndef HUDMANAGER
#define HUDMANAGER
#include <SDL_ttf.h>
#include "MeshManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
// hud manager class
// mainly managed the hud elements (collectable counter, ending screen)
// more details in .cpp file

class hudManager {
public:
	// functions provided in the labs
	GLuint textToTexture(const char * str, GLuint textID);
	void clearTextTexture(GLuint textID);
	hudManager(); // constructor, initializes values
	void renderToHud(int value, GLuint shader, Model *modelData, glm::vec3 pos);


private:
	TTF_Font * textFont;
	GLuint label;

};
#endif