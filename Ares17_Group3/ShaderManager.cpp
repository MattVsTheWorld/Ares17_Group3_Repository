#include "ShaderManager.h"

using namespace std;

/*
Based on rt3d.cpp from Real Time 3d graphics module
*/
namespace ShaderManager {

	// Something went wrong - print error message and quit
void exitFatalError(const char *message) {
    cout << message << " ";
    exit(1);
}

// loadFile - loads text file from file fname as a char* 
// Allocates memory - so remember to delete after use
// size of file returned in fSize
char* loadFile(const char *fname, GLint &fSize) {
	int size;
	char * memblock;

	// file read based on example in cplusplus.com tutorial
	// ios::ate opens file at the end
	ifstream file (fname, ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
		size = (int) file.tellg(); // get location of file pointer i.e. file size
		fSize = (GLint) size;
		memblock = new char [size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();
		cout << "file " << fname << " loaded" << endl;
	}
	else {
		cout << "Unable to open file " << fname << endl;
		fSize = 0;
		// should ideally set a flag or use exception handling
		// so that calling function can decide what to do now
		return nullptr;
	}
	return memblock;
}

// printShaderError
// Display (hopefully) useful error messages if shader fails to compile or link
void printShaderError(const GLint shader) {
	int maxLength = 0;
	int logLength = 0;
	GLchar *logMessage;

	// Find out how long the error message is
	if (!glIsShader(shader))
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0) { // If message has some contents
		logMessage = new GLchar[maxLength];
		if (!glIsShader(shader))
			glGetProgramInfoLog(shader, maxLength, &logLength, logMessage);
		else
			glGetShaderInfoLog(shader,maxLength, &logLength, logMessage);
		cout << "Shader Info Log:" << endl << logMessage << endl;
		delete [] logMessage;
	}
	// should additionally check for OpenGL errors here
}


GLuint initShaders(const char *vertFile, const char *fragFile) {
	GLuint p, f, v;

	char *vs,*fs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);	

	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	vs = loadFile(vertFile,vlen);
	fs = loadFile(fragFile,flen);
	
	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,&vlen);
	glShaderSource(f, 1, &ff,&flen);
	
	GLint compiled;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << vertFile << " : Vertex shader not compiled." << endl;
		ShaderManager::printShaderError(v);
	} 

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << fragFile << " : Fragment shader not compiled." << endl;
		ShaderManager::printShaderError(f);
	} 
	
	p = glCreateProgram();
		
	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	glUseProgram(p);

	delete [] vs; // dont forget to free allocated memory
	delete [] fs; // we allocated this in the loadFile function...

	return p;
}

GLuint initShaders(const char *vertFile, const char *fragFile, const char *geomFile) {
	GLuint p, f, v, g;

	char *vs, *fs, *gs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	g = glCreateShader(GL_GEOMETRY_SHADER);

	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	GLint glen;
	vs = loadFile(vertFile, vlen);
	fs = loadFile(fragFile, flen);
	gs = loadFile(geomFile, glen);

	const char * vv = vs;
	const char * ff = fs;
	const char * gg = gs;

	glShaderSource(v, 1, &vv, &vlen);
	glShaderSource(f, 1, &ff, &flen);
	glShaderSource(g, 1, &gg, &glen);

	GLint compiled;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << vertFile << " : Vertex shader not compiled." << endl;
		ShaderManager::printShaderError(v);
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << fragFile << " : Fragment shader not compiled." << endl;
		ShaderManager::printShaderError(f);
	}

	glCompileShader(g);
	glGetShaderiv(g, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << geomFile << " : Geometry shader not compiled." << endl;
		ShaderManager::printShaderError(g);
	}

	p = glCreateProgram();

	glAttachShader(p, v);
	glAttachShader(p, f);
	glAttachShader(p, g);

	glBindAttribLocation(p, LOCATION_VERTEX, "in_Position");
	glBindAttribLocation(p, LOCATION_COLOUR, "in_Color");
	glBindAttribLocation(p, LOCATION_NORMAL, "in_Normal");
	glBindAttribLocation(p, LOCATION_TEXCOORD, "in_TexCoord");
	glBindAttribLocation(p, LOCATION_BONEID, "in_BoneID");
	glBindAttribLocation(p, LOCATION_BONEWEIGHT, "in_BoneWeigh t");

	glLinkProgram(p);
	glUseProgram(p);

	delete[] vs; // dont forget to free allocated memory
	delete[] fs; // we allocated this in the loadFile function...
	delete[] gs;

	return p;
}
}