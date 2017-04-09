#pragma once

#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "SDL.h"

#include <stack>
#include <string>
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
//+++
#include "SoundManager.h"
#include <time.h>
#include "Projectile.h"
#include "NPCDecorator.h"
//+++

#include "GlobalData.h"

#include <unordered_map>

#define DEG_TO_RADIAN 0.017453293f
#define PI 3.14159265359f

#define MODEL_SCALING 0.01f
#define BOUNDING_SCALING 0.05f


enum gameState {
	RUNNING, PAUSE, MENU, DEFEAT
};

enum bound { 
	BOX, SPHERE, CAPSULE 
};

enum editStages { 
	MODEL, BOUNDING 
};

enum modes { 
	PLAY, EDIT
};

namespace SceneManager {

	void init(SDL_Window * window);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	//fps counter; 3 of 5
	void draw(SDL_Window *window);//, int fps);
}

#endif