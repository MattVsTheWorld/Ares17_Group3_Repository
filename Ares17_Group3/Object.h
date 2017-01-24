#ifndef OBJECT
#define OBJECT
#include "MeshManager.h"
//#include "rt3dObjLoader.h"
#include "loadBitmap.h"
#include <vector>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

enum objectState { STILL, MOVING };

struct transformation_Matrices {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 pitch;
	glm::vec3 yaw;
	glm::vec3 roll;
};

struct physics_Properties {
	float mass;
	float bounciness = 0.0f;
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	bool isOnGround = false;
};

struct object_Properties {
	vector<GLfloat> verts; // contains vertices of loaded object
	vector<GLfloat> norms; // contains normal of loaded object
	vector<GLfloat> tex_coords; // contains texture coordinates of loaded object
	vector<GLuint> indices; // contains indices of loaded object

};

class Object {
private:
	// cube is used for the walls and the skybox
//	object_Properties obj_p;
	GLuint meshIndexCount = 0;
	GLuint texture;
	GLuint meshObject;
	
	//current pitch,yaw,angle, of object
	float pitchAngle;
	float yawAngle;
	float rollAngle;

	transformation_Matrices trans_m;
	physics_Properties phys_p;

	objectState obj_s = STILL;

protected:

public:
	// deprecated constructor
	Object(transformation_Matrices transformation);
	Object(transformation_Matrices transformation, object_Properties obj_p);
	Object(transformation_Matrices transformation, char *texturePath, object_Properties obj_p);
	~Object();

	object_Properties initializeObject(char *objectPath);
	std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
		MeshManager::lightStruct light, MeshManager::materialStruct material, float pitch, float yaw, float roll);
	GLuint object_getIndex();
	GLuint object_getTexture();
	GLuint object_getMesh();
	void setPosition(glm::vec3 newPos);
	glm::vec3 getPosition();
	void setVelocity(glm::vec3 newVel);
	glm::vec3 getVelocity();
	glm::vec3 getScale();

	objectState getState();
	void setState(objectState state);
	
	// NEW +++ (unnecessary)
	//Object copy(Object toCopy);
};

#endif
