#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "MeshManager.h"
#include "SDL.h"
#include "Object.h"
#include "hudManager.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Skybox.h"
#include "Physics.h"
#define DEG_TO_RADIAN 0.017453293

namespace SceneManager {
	void init(void);
	void update(SDL_Window * window);
	void draw(SDL_Window *window, float fps);
}

#endif