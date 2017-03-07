#ifndef NonPC
#define NonPC
#include "AbstractNPC.h"


// NPC implements AbstractNPC - all methods defined inline
class NPC : public AbstractNPC {
private:
	//string name;
	// ++
	btShapeManager *shapeManager;
	btRigidBody* npcBody;
	btPairCachingGhostObject* npcGhost;
	// +++++++++++++++
	bool findCollision(btPairCachingGhostObject* ghostObject) {
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray =
			ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; ++i) {
			manifoldArray.clear();
			const btBroadphasePair& pair = pairArray[i];
			btBroadphasePair* collisionPair = shapeManager->findWorldPair(pair);
			if (!collisionPair) continue;
			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
			for (int j = 0; j < manifoldArray.size(); j++) {
				btPersistentManifold* manifold = manifoldArray[j];
				bool isFirstBody = manifold->getBody0() == ghostObject;
				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);
				for (int p = 0; p < manifold->getNumContacts(); ++p) {
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.f) {
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						// <START>  handle collisions here
						return true;
						//  <END>   handle collisions here
					}
				}
			}
		}
		return false;
	} //TODO: use somewhere
public:

	btRigidBody* addBoundingCapsule(float rad, float height, float x, float y, float z, float mass) {

		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(x, y, z));
		btCapsuleShape* capsule = new btCapsuleShape(rad, height);
		btVector3 inertia(0, 0, 0);
		if (mass != 0.0)
			capsule->calculateLocalInertia(mass, inertia);

		btMotionState* motion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, capsule, inertia);
		btRigidBody* body = new btRigidBody(info);
		body->setAngularFactor(0);
		shapeManager->addToWorld(body, COL_ENEMY, COL_BULLET | COL_PLAYER | COL_DEFAULT); // Can add COL_ENEMY for enemy to enemy collision

		return body;
	}

	NPC(double _h, double _r, btShapeManager* _sm, glm::vec3 spawn, float radius, float height, float mass /*other parameters*/) {
		health = _h;
		range = _r;
		shapeManager = _sm;
		// Construct body
		btRigidBody *temp = addBoundingCapsule(radius, height, spawn.x, spawn.y, spawn.z, mass);
		btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
		tempGhost->setCollisionShape(temp->getCollisionShape());
		tempGhost->setWorldTransform(temp->getWorldTransform());
		tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		shapeManager->addGhostToWorld(tempGhost, COL_ENEMY, COL_BULLET | COL_PLAYER | COL_DEFAULT); // Can add COL_ENEMY for enemy to enemy collision
	}
	void render(glm::mat4 view, glm::mat4 proj, Model* boundingModel, Model * modelData, GLuint shader, GLuint texture) { 
		// Bounding box
		this->shapeManager->renderCapsule(npcBody, view, proj, boundingModel, shader, texture);
		//TODO: actual model

	} //TODO: actual model
	
	double getRange() { return range; }
	double getHealth() { return health; }
	~NPC() { /*cout << "Deleting NPC object " << name << endl;*/ } //TODO: destructor
protected:
//	int health;
//	int mana;
	// ++
	double health;
	double range;
	// (...) space to add more parameters...
};

#endif