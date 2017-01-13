#ifndef SDLMANAGER
#define SDLMANAGER

#include <SDL.h>
#include <iostream>

namespace SDLmanager {

	// Something went wrong - print error message and quit
	void exitFatalError(char *message);
	SDL_Window * setupRC(SDL_GLContext &context);


}

#endif