#include <SDL.h>
#include <iostream>
#include "SDLmanager.h"
#include "SceneManager.h"

#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif


// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[])
{
	SDL_GLContext glContext; // OpenGL context handle
	SDL_Window * hWindow; // window handle

	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		SDLmanager::exitFatalError("Unable to initialize SDL");


							  // Create 800x600 window
	hWindow = SDL_CreateWindow("SDL Test",
		100, 100, 800, 600,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!hWindow) // Check window was created OK
		SDLmanager::exitFatalError("Unable to create window");


	glContext = SDL_GL_CreateContext(hWindow);

	SDL_Event sdlEvent;	// variable to detect SDL events
	bool running = true;
	while (running)		// the event loop
	{
		SDL_PollEvent(&sdlEvent);
		running = SDLmanager::handleSDLEvent(sdlEvent);
		SceneManager::update();		
		SceneManager::draw(hWindow);		
	}


	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
