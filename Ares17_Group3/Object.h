#ifndef OBJECT
#define OBJECT
#include "MeshManager.h"
#include "rt3dObjLoader.h"
#include "loadBitmap.h"
#include <vector>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	Object(char *texturePath);
	~Object();
	std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
		MeshManager::lightStruct light, MeshManager::materialStruct material,
		glm::vec3 transVec, glm::vec3 scaleVec, glm::vec3 rotateVec, float angle);
	GLuint object_getIndex();
	GLuint object_getTexture();
	GLuint object_getMesh();

	// I need a blue banana

};

#endif