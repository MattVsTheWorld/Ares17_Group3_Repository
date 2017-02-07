#ifndef SHADERMANAGER
#define SHADERMANAGER

#include <GL/glew.h>
#include <iostream>
#include <fstream>

#define LOCATION_VERTEX		0
#define LOCATION_COLOUR		1
#define LOCATION_NORMAL		2
#define LOCATION_TEXCOORD   3
#define LOCATION_INDEX		4

namespace ShaderManager {
	
	void exitFatalError(const char *message);
	char* loadFile(const char *fname, GLint &fSize);
	void printShaderError(const GLint shader);
	GLuint initShaders(const char *vertFile, const char *fragFile);
	GLuint initShaders(const char *vertFile, const char *fragFile, const char *geomFile);

}

#endif