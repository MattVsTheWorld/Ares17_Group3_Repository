#ifndef NONPC
#define NONPC
#include "AbstractNPC.h"

#define PI 3.14159265359f
#define REFRESHRATE 1.0f
#define DEFAULT_LOS 20.0f

// NPC implements AbstractNPC - all methods defined inline
class NonPC : public AbstractNPC {
private:

	float angle;
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

	void changeSpeed(float speed, float angle) {
		//cout << angle << endl;
		//TODO: fix
		this->npcBody->setLinearVelocity(btVector3(/*npcBody->getLinearVelocity().x() + */speed*std::cos(angle),
			npcBody->getLinearVelocity().y(), /*npcBody->getLinearVelocity().z()*/ speed*std::sin(angle)));
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
		body->setAngularFactor(btVector3(0, 1, 0));
		shapeManager->addToWorld(body, COL_ENEMY, COL_PLAYER | COL_DEFAULT | COL_ENEMY); // Can add COL_ENEMY for enemy to enemy collision

		return body;
	}

	NonPC(double _h, double _r, btShapeManager* _sm, glm::vec3 _spawn, float radius, float height, float mass,
		Model* _bmodel, GLuint _shader, GLuint _text /*other parameters*/) {
		health = _h;
		max_hp = _h;
		range = _r;
		shapeManager = _sm;
		boundingModel = _bmodel;

		spawn = _spawn;
		//shader = _shader;
		texture = _text;
		// Construct body
		btRigidBody *temp = addBoundingCapsule(radius, height, spawn.x, spawn.y, spawn.z, mass);
		btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
		tempGhost->setCollisionShape(temp->getCollisionShape());
		tempGhost->setWorldTransform(temp->getWorldTransform());
		tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		shapeManager->addGhostToWorld(tempGhost, COL_ENEMY, COL_BULLET);
		npcBody = temp;
		npcGhost = tempGhost;

	}

	~NonPC() {
		shapeManager->removeObject(npcBody);
		shapeManager->removeObject(npcGhost);
		//	delete shapeManager;
		delete npcBody;
		delete npcGhost;
		cout << "Done deleting enemy object" << endl;
	}

	void moveNpc(vertex* v) {
		btVector3 goTo(v->getCoords().first, 0, v->getCoords().second);
		btTransform t;
		t.setIdentity();
		npcBody->getMotionState()->getWorldTransform(t);
		btVector3 pos = t.getOrigin();

		//	cout << "Going to: " << v->getIndex() << endl;
		angle = (atan2(goTo.z() - pos.z(), goTo.x() - pos.x())); // RADIANS
		changeSpeed(8, angle);

		// find angle between direction
		//changeSpeed(newDir);
	}

	//TODO: search only if reachable
	queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId) {
		A_star *pathfinder = new A_star(adjList);
		//	cout << "////////\n" << startId << " to " << endId << endl;
		list<vertex*> path = pathfinder->algorithm_standard(adjList->getVertex(startId), adjList->getVertex(endId));
		queue<vertex*> toVisit;
		for (const auto &pathIterator : path) {
			//		cout << " " << pathIterator->getIndex() << " ";
			toVisit.push(static_cast<vertex*>(pathIterator));
		}
		adjList->resetCosts();
		delete pathfinder;
		return toVisit;
	} // findPath function

	bool update(std::tuple<Model*, Model*, Model*> modelDatas, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player *player, GLuint shader) {

		//TODO: unstuck
		btVector3 playerPos(player->getPosition().x, player->getPosition().y, player->getPosition().z);
		btTransform t;
		t.setIdentity();
		npcBody->getMotionState()->getWorldTransform(t);
		btVector3 pos = t.getOrigin();

		Model* currentAnimation = get<2>(modelDatas);

		if ((this->currentState != PAUSED && this->currentState != DYING) && sqrt(pow(playerPos.x() - pos.x(), 2) + pow(playerPos.z() - pos.z(), 2) <= DEFAULT_LOS))
			this->currentState = TRIGGERED;

		if (this->currentState == TRIGGERED || this->currentState == ATTACKING) {
			
			if (init) {
				currentPath = findPath(_g->getAdjList(), _g->getNodeFromWorldPos(pos), _g->getNodeFromWorldPos(playerPos));
				init = false;
			}
			if (sqrt(pow(playerPos.x() - pos.x(), 2) + pow(playerPos.z() - pos.z(), 2)) >= this->range) { // distance
				this->currentState = TRIGGERED;
				recalcTimer -= dt;
				if (recalcTimer <= 0)
				{
					currentPath = findPath(_g->getAdjList(), _g->getNodeFromWorldPos(pos), _g->getNodeFromWorldPos(playerPos));
					recalcTimer = REFRESHRATE;
				}

				if (!currentPath.empty())
					if (_g->getAdjList()->getVertex(_g->getNodeFromWorldPos(pos)) == currentPath.front())
						currentPath.pop();
				if (!currentPath.empty()) {
					this->moveNpc(currentPath.front());
					currentAnimation = get<0>(modelDatas); //running animation
				}
			} else {
				this->currentState = ATTACKING;
				//Attack player
				//player->varyHealth(-(this->attack));
				//TODO: MESHAL attack animation
				currentAnimation = get<1>(modelDatas); //attack animation
				if (this->attackTimer <= 0) {
					player->takeDamage(this->attack);
					this->attackTimer = this->attackSpeed;
				}
				else this->attackTimer -= dt;
			}
		}
		if (currentState == DYING){
			currentAnimation = get<2>(modelDatas);
		}

		this->render(currentAnimation, view, proj, shader);

			if (findCollision(npcGhost))
			{
				this->health -= player->getWeapon().getDamage();
				/*if (player->getWeapon() == PISTOL || player->getWeapon() == NUKA)
					this->health -= 10;
				else if (player->getWeapon() == SCIFI)
					this->health -= 40;*/
				cout << "HIT! Health = " << this->health << endl;
			}

		return true;
	}

	void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader) {
		//	glUseProgram(shader);
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
		model = glm::rotate(model, std::atan2(std::cos(angle), std::sin(angle)), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.7, 0.7, 0.7));	// It's a bit too big for our scene, so scale it down
	//	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// for gun]
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader, "animated"), 1); //zero is no animations
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

		glBindTexture(GL_TEXTURE_2D, 0);
		//TODO: actual model

	} //TODO: actual model

	void setAttack(double atk) { this->attack = atk; }
	double getAttack() { return this->attack; }
	void modifyHealth(double newHp) { this->health += newHp; }
	double getRange() { return range; }
	void setRange(double rng) { this->range = rng; }
	double getHealth() { return health; }
	void setAttackSpeed(double atkspd) {
		this->attackSpeed = atkspd;
	}
	double getAttackSpeed() {
		return this->attackSpeed;
	}
	void setName(string _name) { name = _name; }
	string getName() { return name; }

	void setState(npcState newState) { this->currentState = newState; }
	npcState getState() { return this->currentState; }

	void reset() {
	/*	this->health = this->max_hp;
		btTransform t;
		t.setIdentity();
		npcBody->getMotionState()->getWorldTransform(t);
		t.setOrigin(btVector3(spawn.x, spawn.y, spawn.z));
		this->npcBody->setWorldTransform(t);
		this->npcBody->setLinearVelocity(btVector3(0.5, -0.5, 0.0));*/
		//;
	}
protected:
	//string name;
	// ++
	btShapeManager *shapeManager;
	btRigidBody* npcBody;
	btPairCachingGhostObject* npcGhost;
	//
	Model * boundingModel;
	// Model
	//GLuint shader;
	GLuint texture;
	/// ++++
	queue<vertex*> currentPath;
	double recalcTimer = REFRESHRATE;
	double attackTimer = 0;
	glm::vec3 spawn;
	double max_hp;
	double health;
	double range;
	double attack;
	double attackSpeed;
	bool init = true;
	string name;
	npcState currentState = IDLE;
	// (...) space to add more parameters...
};

#endif