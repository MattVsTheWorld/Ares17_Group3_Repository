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


	void renderToHud(int N, GLuint shader, MeshManager::lightStruct light,
		GLuint meshObject, GLuint meshIndexCount, glm::vec3 pos, float value_x, float value_y, float value_z);


private:
	TTF_Font * textFont;
	GLuint label;

};
#endif