#ifndef NONPC
#define NONPC
#include "AbstractNPC.h"

#define PI 3.14159265359f

// NPC implements AbstractNPC - all methods defined inline
class NonPC : public AbstractNPC {
private:
	//string name;
	// ++
	btShapeManager *shapeManager;
	btRigidBody* npcBody;
	btPairCachingGhostObject* npcGhost;
	//
	Model * boundingModel;
	// Model
	GLuint shader;
	GLuint texture;

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

	static void toEulerianAngle(const btQuaternion& q, glm::vec3& rotation)
	{
		double test = q.x()*q.y() + q.z()*q.w();
		if (test > 0.499) { // singularity at north pole
			rotation.y = 2 * atan2(q.x(), q.w());
			rotation.x = PI / 2.0f;
			rotation.z = 0;
			return;
		}
		if (test < -0.499) { // singularity at south pole
			rotation.y = -2 * atan2(q.x(), q.w());
			rotation.x = -PI / 2.0f;
			rotation.z = 0;
			return;
		}
		double sqx = q.x()*q.x();
		double sqy = q.y()*q.y();
		double sqz = q.z()*q.z();
		rotation.y = atan2(2 * q.y()*q.w() - 2 * q.x()*q.z(), 1 - 2 * sqy - 2 * sqz);
		rotation.x = asin(2 * test);
		rotation.z = atan2(2 * q.x()*q.w() - 2 * q.y()*q.z(), 1 - 2 * sqx - 2 * sqz);
	}

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

	NonPC(double _h, double _r, btShapeManager* _sm, glm::vec3 spawn, float radius, float height, float mass,
		Model* _bmodel, GLuint _shader, GLuint _text /*other parameters*/) {
		health = _h;
		range = _r;
		shapeManager = _sm;
		boundingModel = _bmodel;
		shader = _shader;
		texture = _text;
		// Construct body
		btRigidBody *temp = addBoundingCapsule(radius, height, spawn.x, spawn.y, spawn.z, mass);
		btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
		tempGhost->setCollisionShape(temp->getCollisionShape());
		tempGhost->setWorldTransform(temp->getWorldTransform());
		tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		shapeManager->addGhostToWorld(tempGhost, COL_ENEMY, COL_BULLET); // Can add COL_ENEMY for enemy to enemy collision
		npcBody = temp;
		npcGhost = tempGhost;
	}

	~NonPC() {
		shapeManager->removeObject(npcBody);
		shapeManager->removeObject(npcGhost);
	//	delete shapeManager;
		delete npcBody;
		delete npcGhost;
		cout << "Done deleting enemy objects" << endl;
	}

	bool update(Model * modelData, glm::mat4 view, glm::mat4 proj) {
			this->render(modelData, view, proj);
			return true;
	}
	void render(Model * modelData, glm::mat4 view, glm::mat4 proj) {


			if (findCollision(npcGhost))
			{
				this->health -= 10;
				cout << "HIT! Health = " << this->health << endl;
			}

			btTransform t;
			t.setIdentity();
			npcBody->getMotionState()->getWorldTransform(t);
			btVector3 pos = t.getOrigin();
			npcGhost->setWorldTransform(t);
			// Bounding box
			//TODO: fix (or feature)
			//this->shapeManager->renderCapsule(npcBody, view, proj, boundingModel, shader, texture);

			btQuaternion& rotation = npcBody->getWorldTransform().getRotation().normalized();
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(pos.x(), pos.y() - 1.75, pos.z()));
			glm::vec3 eulerRotation;
			toEulerianAngle(rotation, eulerRotation);
			model = glm::rotate(model, eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.15, 0.15, 0.15));	// It's a bit too big for our scene, so scale it down
												//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// for gun]
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, texture);
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			modelData->Draw(shader);

			glBindTexture(GL_TEXTURE_2D, 0);
		//TODO: actual model

	} //TODO: actual model

	void modifyHealth(double newHp) { this->health += newHp; }
	double getRange() { return range; }
	double getHealth() { return health; }
protected:
	//	int health;
	//	int mana;
		// ++
	double health;
	double range;
	// (...) space to add more parameters...
};

#endif