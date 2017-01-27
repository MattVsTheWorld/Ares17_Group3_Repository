#ifndef BTSHAPEMANAGER
#define BTSHAPEMANAGER

#include <btBulletDynamicsCommon.h>
#include "MeshManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Class to manage shapes created with the bullet physics library
class btShapeManager {
	btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);
	btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	void renderSphere(btRigidBody* sphere, glm::mat4 proj, Model *modelData, MeshManager::materialStruct material);
	void renderBox(btRigidBody* box, glm::mat4 proj, Model *modelData, MeshManager::materialStruct material);

private:
	std::vector<btRigidBody*> bodies;
};
#endif