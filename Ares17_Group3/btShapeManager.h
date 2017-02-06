#ifndef BTSHAPEMANAGER
#define BTSHAPEMANAGER

#include <btBulletDynamicsCommon.h>
#include "MeshManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//test
#include "loadBitmap.h"

// Class to manage shapes created with the bullet physics library

// shader doesn't change, can save as variable
// Undecided on light yet; that might change.
// Add set/ get light?

struct btSettings {
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase; //improves collision check (?) // can improve by knowing world size (?)
	btConstraintSolver* solver;
};

class btShapeManager {
public:
	btShapeManager();
	void update();
	btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);
	btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
//	btRigid
	void renderSphere(btRigidBody* sphere, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader);
	void renderBox(btRigidBody* box, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader);
	void renderCapsule(btRigidBody* capsule, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader);
	void addToWorld(btRigidBody* body);

private:	
	btSettings btSettings;
	//std::vector<btRigidBody*> bodies;
	//GLuint modelProgram;
	//MeshManager::lightStruct light;
	GLuint defaultTexture;

};
#endif