#ifndef SDLMANAGER
#define SDLMANAGER

#include <SDL.h>
#include <iostream>

// http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement
// How many frames time values to keep
// The higher the value the smoother the result is...
// Don't make it 0 or less :)
#define FRAME_VALUES 10

namespace SDLmanager {

	// Something went wrong - print error message and quit
	void exitFatalError(char *message);
	SDL_Window * setupRC(SDL_GLContext &context);


}

#endif