// rt3dObjLoader.h taken from the labs
// A simple OBJ loader
// (c) Daniel Livingstone 2013
// Please don't share - this was hacked together quite quickly and is for demonstration use only
//
// Limitations:
// Only works with triangulated OBJ models
// Does not support groups or multiple meshes per file
// Does not support anything other than very straightforward OBJ models
// Will not generate normals if the model is missing them - or any other missing data
#ifndef RT3D_OBJ_LOADER
#define RT3D_OBJ_LOADER

#include <GL/glew.h>
#include <vector>

namespace rt3d {

	void loadObj(const char* filename, std::vector<GLfloat> &verts, std::vector<GLfloat> &norms, 
		std::vector<GLfloat> &texcoords, std::vector<GLuint> &indices);

}

#endif