#ifndef BTSHAPEMANAGER
#define BTSHAPEMANAGER

#include <btBulletDynamicsCommon.h>
#include "MeshManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GRAVITY -10

//TODO:move
#define BIT(x) (1<<(x))
enum collisiontype {
	COL_NOTHING = 0,
	COL_DEFAULT = BIT(0),
	COL_PLAYER = BIT(1),
	COL_BULLET = BIT(2),
	COL_ENEMY = BIT(3)

	
	//(...)
};
#define COLLIDE_ALL COL_DEFAULT | COL_PLAYER | COL_BULLET | COL_ENEMY
//!!

// Settings struct
struct btSettings {
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase; //improves collision check (?) // can improve by knowing world size (?)
	btConstraintSolver* solver;
};
// Class to manage shapes created with the bullet physics library
class btShapeManager {
public:
	btShapeManager();
	void update();
	btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);
	btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	btRigidBody* addCapsule(float rad, float height, float x, float y, float z, float mass);
	void renderSphere(btRigidBody* sphere, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture);
	void renderBox(btRigidBody* box, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture);
	void renderCapsule(btRigidBody* capsule, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture);
	void addToWorld(btRigidBody* body);
	void addToWorld(btRigidBody* body, collisiontype COLL_TYPE, int collidesWith);
	void addGhostToWorld(btPairCachingGhostObject* ghost, collisiontype COLL_TYPE, int collidesWith);
	void addGhostToWorld(btPairCachingGhostObject* ghost);
	btBroadphasePair* findWorldPair(const btBroadphasePair &pair);

private:	
	btSettings btSettings;

};
#endif