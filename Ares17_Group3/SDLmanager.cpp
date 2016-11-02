#include "SDLmanager.h"



namespace SDLmanager {
	// Something went wrong - print error message and quit
	void exitFatalError(char *message)
	{
		std::cout << message << " " << std::endl;
		std::cout << SDL_GetError();
		SDL_Quit();
		exit(1);
	}

	bool handleSDLEvent(SDL_Event const &sdlEvent)
	{
		if (sdlEvent.type == SDL_QUIT)
			return false;
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			// Can extend this to handle a wider range of keys
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return false;
			default:
				break;
			}
		}
		return true;
	}

}