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

	SDL_Window * setupRC(SDL_GLContext &context) {
		SDL_Window * window;
		if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
			exitFatalError("Unable to initialize SDL");

		// Request an OpenGL 3.0 context.
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

														   // Create 800x600 window
		window = SDL_CreateWindow("Ares 17", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (!window) // Check window was created OK
			exitFatalError("Unable to create window");

		context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
		SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
		return window;
	}


}