#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "SDL.h"

#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDLmanager.h"
#include "hudManager.h"
#include "Skybox.h"
#include "btShapeManager.h"
#include "Player.h"
// TEMPORARY
#include "loadBitmap.h"
#define DEG_TO_RADIAN 0.017453293

namespace SceneManager {
	void init(void);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	//fps counter; 3 of 5
	void draw(SDL_Window *window);//, int fps);
}

#endif