#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "MeshManager.h"
#include "SDL.h"
#include "Object.h"

namespace SceneManager {
	void update(void);
	void draw(SDL_Window *window);
}

#endif