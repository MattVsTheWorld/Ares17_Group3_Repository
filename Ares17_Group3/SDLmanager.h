#ifndef SDLMANAGER
#define SDLMANAGER

#include <SDL.h>
#include <iostream>

#define SCREENWIDTH 1440.0f
#define SCREENHEIGHT 900.0f

namespace SDLmanager {

	// Something went wrong - print error message and quit
	void exitFatalError(char *message);
	SDL_Window * setupRC(SDL_GLContext &context);


}

#endif