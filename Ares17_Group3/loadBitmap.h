#ifndef LOADBITMAP
#define LOADBITMAP
#include <GL/glew.h>
#include "SDL.h"
#include <iostream>

// just created a simple namespace for the load bitmap function
namespace loadBitmap
{
	GLuint loadBitmap(char *fname);
}
#endif