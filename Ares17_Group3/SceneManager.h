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
#include "loadBitmap.h"
#include "SoundManager.h"
#include <time.h>
#include "Projectile.h"
#include "NPCDecorator.h"
#include "GlobalData.h"

#include <unordered_map>

#define DEG_TO_RADIAN 0.017453293f
#define PI 3.14159265359f

#define MODEL_SCALING 0.01f
#define BOUNDING_SCALING 0.05f


enum gameState {
	RUNNING, PAUSE, MENU, DEFEAT, VICTORY
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

enum level {
	FIRST,
	SECOND,
	BOSS
};

namespace SceneManager {
	// Main functions needed by main.cpp
	void init(SDL_Window * window);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	void draw(SDL_Window *window);
}

#endif