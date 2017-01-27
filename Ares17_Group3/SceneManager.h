#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "MeshManager.h"
#include "SDL.h"
//#include "Object.h"
#include "hudManager.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Skybox.h"
//#include <time.h>

#include <btBulletDynamicsCommon.h>

// TEMPORARY
#include "loadBitmap.h"

//#include "Bullet.h"
#include "Player.h"
#define DEG_TO_RADIAN 0.017453293

namespace SceneManager {
	void init(void);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	void draw(SDL_Window *window);
}

#endif