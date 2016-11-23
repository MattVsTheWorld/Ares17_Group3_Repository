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

struct transformation_Matrices {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

struct physics_Properties {
	float mass;
	float bounciness = 0.0f;
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	bool isOnGround = false;
};

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
	
	float rotationAngle;
	transformation_Matrices trans_m;
	physics_Properties phys_p;

	float angleAtShot;
	glm::vec3 at;
	glm::vec3 initialPos;

protected:

public:
	// deprecated constructor
	Object(transformation_Matrices transformation);
	Object(transformation_Matrices transformation, char *texturePath);
	~Object();
	std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
		MeshManager::lightStruct light, MeshManager::materialStruct material, float rot);
	GLuint object_getIndex();
	GLuint object_getTexture();
	GLuint object_getMesh();
	void setPosition(glm::vec3 newPos);
	glm::vec3 getPosition();
	void setVelocity(glm::vec3 newVel);
	glm::vec3 getVelocity();

	glm::vec3 getScale();

	float getAngle();
	void setAngle(float angle);

	////EXPERIMENTING
	void setAt(glm::vec3 newAt);
	glm::vec3 getAt();

	void setInitialPosition(glm::vec3 newPos);
	glm::vec3 getInitialPosition();
};

#endif