#include "SceneManager.h"

namespace SceneManager {

	void update(void) {

	}

	void draw(SDL_Window * window) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window


		SDL_GL_SwapWindow(window); // swap buffers
	}
}