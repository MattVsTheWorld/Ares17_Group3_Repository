#ifndef PROJECTILE
#define PROJECTILE

#include "btShapeManager.h"
#define BULLET_SIZE 0.05f
#define DEFAULT_LIFESPAN 6.0f
#define DEFAULT_MASS 1.0f
#define PROJ_SPEED 20.0f
#define PITCHLOCK 3.0f

typedef tuple<btRigidBody*, double, btPairCachingGhostObject*> _proj;

class Projectile {
	



private:
	btShapeManager *shapeManager;
	vector <_proj> *liveProjectiles;
	
protected:
public:
	Projectile(btShapeManager *s_Manager) {
		this->shapeManager = s_Manager;
		liveProjectiles = new vector <_proj>;
		//liveProjectiles.push_back(make_pair(shapeManager->addSphere(10, 0, 0, 0, 10), this->lifespan));
	}

	btRigidBody* addSphere(float rad, float x, float y, float z, float mass)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(x, y, z));
		btSphereShape* sphere = new btSphereShape(rad);
		btVector3 inertia(0, 0, 0);
		if (mass != 0.0)
			sphere->calculateLocalInertia(mass, inertia);

		btMotionState* motion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
		btRigidBody* body = new btRigidBody(info);
		shapeManager->addToWorld(body, COL_NOTHING, COL_NOTHING);
		//bodies.push_back(body);
		return body;
	}

	void addProjectile(glm::vec3 spawn, float speed, float yaw, float pitch) {
		btRigidBody *temp = addSphere(BULLET_SIZE, spawn.x, spawn.y, spawn.z, DEFAULT_MASS);
		btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();

		// mask
		// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Filtering
		int coll = COL_CANHIT;
		int noColl = COL_NOTHING;

		tempGhost->setCollisionShape(temp->getCollisionShape());
		tempGhost->setWorldTransform(temp->getWorldTransform());
		tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		shapeManager->addGhostToWorld(tempGhost);
		liveProjectiles->push_back(make_tuple(temp, DEFAULT_LIFESPAN, tempGhost));
		if ((pitch) >= PITCHLOCK / 2)
			pitch = PITCHLOCK / 2;
		if (pitch <= -(PITCHLOCK / 2))
			pitch = -PITCHLOCK / 2;
	/*	cout << "x" << 1*std::sin(yaw) << endl;
		cout << "y" << -1*std::sin(pitch) << endl;
		cout << "z" << -1*std::cos(yaw) << endl;*/
		// TODO: fix angles. yaw and pitch are ""unrelated"", creating the unwanted effect that shooting up won't reduce ground-parallel speed
		// ALSO: Spawn point is wrong
		temp->setLinearVelocity(btVector3(speed*std::sin(yaw), -speed*std::sin(pitch), -speed*std::cos(yaw)));
	}
	~Projectile() {
		delete this; // :thinking:
	}
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture, double time_step) {
		vector <_proj>::iterator projectileIterator = liveProjectiles->begin();
		while (liveProjectiles->size() > 0 && projectileIterator != liveProjectiles->end()) {
			(*projectileIterator) = make_tuple(get<0>(static_cast<_proj>(*projectileIterator)), get<1>(static_cast<_proj>(*projectileIterator)) - time_step, get<2>(static_cast<_proj>(*projectileIterator)));
			// I'M A GENIUS!!!
			get<2>(static_cast<_proj>(*projectileIterator))->setWorldTransform(get<0>(static_cast<_proj>(*projectileIterator))->getWorldTransform());
		
			// OH NO! I'M NOT :(
			 findCollision(get<2>(static_cast<_proj>(*projectileIterator)));
			if (get<1>(static_cast<_proj>(*projectileIterator)) <= 0) // if dead, remove // delete?
				liveProjectiles->erase(remove(liveProjectiles->begin(), liveProjectiles->end(), static_cast<_proj>(*projectileIterator)), liveProjectiles->end());
			else {
				this->shapeManager->renderSphere((get<0>(static_cast<_proj>(*projectileIterator))), view, proj, modelData, shader, texture);
			}
			if (liveProjectiles->size() > 0)
				projectileIterator++; 
			//get<0>(static_cast<_proj>(*projectileIterator));
		}
	}

	//void findBulletCollision() {
	//	vector <_proj>::iterator projectileIterator = liveProjectiles->begin();
	//	while (liveProjectiles->size() > 0 && projectileIterator != liveProjectiles->end()) {
	//		//findCollision((static_cast<_proj>(*projectileIterator)).first);
	//		//if ((static_cast<_proj>(*projectileIterator)).second <= 0) // if dead, remove // delete?
	//		//	liveProjectiles->erase(remove(liveProjectiles->begin(), liveProjectiles->end(), static_cast<_proj>(*projectileIterator)), liveProjectiles->end());
	//		if (liveProjectiles->size() > 0)
	//			projectileIterator++;
	//	}
	//}

	void findCollision(btPairCachingGhostObject* ghostObject) { // TODO: ignore self
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
						cout << "BULLET HIT SOMETHING" << endl;
						//  <END>   handle collisions here
					}
				}
			}
		}
	}
};


#endif