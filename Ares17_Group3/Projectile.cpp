#include "Projectile.h"

Projectile::Projectile(btShapeManager *s_Manager) {
	this->shapeManager = s_Manager;
}

btRigidBody* Projectile::addSphere(float rad, float x, float y, float z, float mass)
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

	shapeManager->addToWorld(body, COL_BULLET, COL_ENEMY | COL_DEFAULT);
	return body;
}

void Projectile::addProjectile(glm::vec3 spawn, float speed, float yaw, float pitch) {
	btRigidBody *temp = addSphere(BULLET_SIZE, spawn.x, spawn.y, spawn.z, DEFAULT_MASS);
	btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
	// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Filtering

	tempGhost->setCollisionShape(temp->getCollisionShape());
	tempGhost->setWorldTransform(temp->getWorldTransform());
	tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	shapeManager->addGhostToWorld(tempGhost, COL_BULLET, COL_ENEMY | COL_DEFAULT);
	liveProjectiles.push_back(make_tuple(temp, DEFAULT_LIFESPAN, tempGhost));
	if ((pitch) >= PITCHLOCK / 2)
		pitch = PITCHLOCK / 2;
	if (pitch <= -(PITCHLOCK / 2))
		pitch = -PITCHLOCK / 2;
	temp->setLinearVelocity(btVector3(speed*std::sin(yaw), -speed*std::sin(pitch), -speed*std::cos(yaw)));
}


void Projectile::renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture, double time_step) {

	unsigned int i = 0;
	while (liveProjectiles.size() > 0 && i < liveProjectiles.size())
	{
		liveProjectiles[i] = make_tuple(get<0>(liveProjectiles[i]), get<1>(liveProjectiles[i]) - time_step, get<2>(liveProjectiles[i]));
		// update ghost
		get<2>(liveProjectiles[i])->setWorldTransform(get<0>(liveProjectiles[i])->getWorldTransform());

		if (findCollision(get<2>(liveProjectiles[i])))
		{
			shapeManager->removeObject(get<0>(liveProjectiles[i])); // Remove from world
			shapeManager->removeObject(get<2>(liveProjectiles[i]));
			liveProjectiles.erase(remove(liveProjectiles.begin(), liveProjectiles.end(), liveProjectiles[i]), liveProjectiles.end());
			continue;
		}

		if (get<1>(liveProjectiles[i]) <= 0)
		{
			shapeManager->removeObject(get<0>(liveProjectiles[i])); // Remove from world
			shapeManager->removeObject(get<2>(liveProjectiles[i]));
			liveProjectiles.erase(remove(liveProjectiles.begin(), liveProjectiles.end(), liveProjectiles[i]), liveProjectiles.end());
		}

		else {
			this->shapeManager->renderSphere((get<0>(liveProjectiles[i])), view, proj, modelData, shader, texture);
		}
		if (liveProjectiles.size() > 0)
			i++;
	}
}

bool Projectile::findCollision(btPairCachingGhostObject* ghostObject) {
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
}