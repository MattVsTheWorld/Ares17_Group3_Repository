#ifndef SCENEMANAGER
#define SCENEMANAGER

//////////////////////
//+++ FPS EXPERIMENT
//////////////////////

//////////////////////
//--- FPS EXPERIMENT
//////////////////////

//#include "MeshManager.h"
#include "SDL.h"
//#include "Object.h"
#include "hudManager.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Skybox.h"

//#include <btBulletDynamicsCommon.h>
#include "btShapeManager.h"
// TEMPORARY
#include "loadBitmap.h"
//#include <map>

//#include "Bullet.h"
#include "Player.h"
#define DEG_TO_RADIAN 0.017453293

namespace SceneManager {
	void init(void);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	void draw(SDL_Window *window);//, int fps);
}

#endif