#ifndef OBJECT
#define OBJECT
#include "MeshManager.h"
#include "rt3dObjLoader.h"
#include "loadBitmap.h"
#include <vector>
using namespace std;

class Object {
private:
	// cube is used for the walls and the skybox
	vector<GLfloat> verts; // contains vertices of loaded object
	vector<GLfloat> norms; // contains normal of loaded object
	vector<GLfloat> tex_coords; // contains texture coordinates of loaded object
	vector<GLuint> indices; // contains indices of loaded object
	GLuint meshIndexCount = 0;
	GLuint texture;
	GLuint meshObject;

protected:

public:
	Object();
	~Object();

	GLuint object_getIndex();
	GLuint object_getTexture();
	GLuint object_getMesh();

	// I need a blue banana

};

#endif