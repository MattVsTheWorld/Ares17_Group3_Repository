#include <SDL.h>
#include <iostream>
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

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



// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[])
{
	SDL_GLContext glContext; // OpenGL context handle
	SDL_Window * hWindow; // window handle

	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		exitFatalError("Unable to initialize SDL");


							  // Create 800x600 window
	hWindow = SDL_CreateWindow("SDL Test",
		100, 100, 800, 600,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!hWindow) // Check window was created OK
		exitFatalError("Unable to create window");


	glContext = SDL_GL_CreateContext(hWindow);

	SDL_Event sdlEvent;	// variable to detect SDL events
	bool running = true;
	while (running)		// the event loop
	{
		SDL_PollEvent(&sdlEvent);
		running = handleSDLEvent(sdlEvent);
		//update();			// not used yet!
		//draw(hWindow);		// not used yet!
	}

	//test
	// aff
	// fff

	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
