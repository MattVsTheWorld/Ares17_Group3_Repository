#include"NonPC.h"


bool NonPC::findCollision(btPairCachingGhostObject* ghostObject) {
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

void NonPC::changeSpeed(float speed, float angle) {
	//cout << angle << endl;
	//TODO: increase speed
	this->npcBody->setLinearVelocity(btVector3(/*npcBody->getLinearVelocity().x() + */speed*std::cos(angle),
		npcBody->getLinearVelocity().y(), /*npcBody->getLinearVelocity().z()*/ speed*std::sin(angle)));
}

btRigidBody* NonPC::addBoundingCapsule(float rad, float height, float x, float y, float z, float mass) {

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

NonPC::NonPC(double _h, double _r, btShapeManager* _sm, glm::vec3 _spawn, float radius, float height, float mass,
	Model* _bmodel, GLuint _shader, GLuint _text, SoundManager *sound_man /*other parameters*/) {
	health = _h;
	range = _r;
	shapeManager = _sm;
	boundingModel = _bmodel;
	s_manager = sound_man;
	spawn = _spawn;
	//shader = _shader;
	texture = _text;
	// Construct body
	btRigidBody *temp = addBoundingCapsule(radius, height, spawn.x, spawn.y, spawn.z, mass);
	temp->setFriction(FRICTION);
	btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
	tempGhost->setCollisionShape(temp->getCollisionShape());
	tempGhost->setWorldTransform(temp->getWorldTransform());
	tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	shapeManager->addGhostToWorld(tempGhost, COL_ENEMY, COL_BULLET);
	npcBody = temp;
	npcGhost = tempGhost;

}

NonPC::~NonPC() {
	shapeManager->removeObject(npcBody);
	shapeManager->removeObject(npcGhost);
	//	delete shapeManager;
	delete npcBody;
	delete npcGhost;
	cout << "Done deleting enemy object" << endl;
}

glm::vec3 NonPC::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(angle * DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle * DEG_TO_RADIAN));
}

void NonPC::moveNpc(vertex* v, float speed) {
	btVector3 goTo(v->getCoords().first, 0, v->getCoords().second);
	btTransform t;
	t.setIdentity();
	npcBody->getMotionState()->getWorldTransform(t);
	btVector3 pos = t.getOrigin();

	//	cout << "Going to: " << v->getIndex() << endl;
	angle = (atan2(goTo.z() - pos.z(), goTo.x() - pos.x())); // RADIANS

	if (moved == false) {
		float rotationAngle = std::atan2(std::cos(angle), std::sin(angle));
		glm::vec3 move = moveForward(glm::vec3(pos.x(), pos.y() + 0.5, pos.z()), rotationAngle, 0.1f);
		t.setOrigin(btVector3(move.x, move.y, move.z));
	}

	npcBody->getMotionState()->setWorldTransform(t);

	changeSpeed(speed, angle);

}

queue<vertex*> NonPC::findPath(AdjacencyList *adjList, int startId, int endId) {
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

bool NonPC::update(Model* modelData, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player *player, GLuint shader, float speed) {

	this->render(modelData, view, proj, shader, player);

	//TODO: unstuck
	btVector3 playerPos(player->getPosition().x, player->getPosition().y, player->getPosition().z);
	btTransform t;
	t.setIdentity();
	npcBody->getMotionState()->getWorldTransform(t);
	btVector3 pos = t.getOrigin();

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

			//TODO: why two of the same if statements
			if (!currentPath.empty())
				if (_g->getAdjList()->getVertex(_g->getNodeFromWorldPos(pos)) == currentPath.front())
					currentPath.pop();
			if (!currentPath.empty()) {
				this->moveNpc(currentPath.front(), speed);
				moved = true;
			}
		}
		else {
			this->currentState = ATTACKING;
			if (this->attackTimer <= 0) {
				player->takeDamage(this->attack);
				s_manager->playSound(s_manager->getSound(PAINED_PL), 1, 1);
				this->attackTimer = this->attackSpeed;
			}
			else this->attackTimer -= dt;
			moved = false;
		}
	}

	if (findCollision(npcGhost))
	{
		this->health -= player->getWeapon().getDamage();
		//cout << "Hit! :" << this->health << endl;
		s_manager->playSound(s_manager->getSound(PAINED_EN), 1, 1);
	}

	return true;
}

void NonPC::render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player) {
	//	glUseProgram(shader);
	btTransform t;
	t.setIdentity();
	npcBody->getMotionState()->getWorldTransform(t);
	btVector3 pos = t.getOrigin();
	npcGhost->setWorldTransform(t);
	// Bounding box
	//this->shapeManager->renderCapsule(npcBody, view, proj, boundingModel, shader, texture);

	btQuaternion& rotation = npcBody->getWorldTransform().getRotation().normalized();
	glm::mat4 model;
	if (this->name == "boss") {
		model = glm::translate(model, glm::vec3(pos.x(), pos.y() - 1.0, pos.z()));
	}
	else
		model = glm::translate(model, glm::vec3(pos.x(), pos.y() - 1.50, pos.z()));
	glm::vec3 eulerRotation;
	toEulerianAngle(rotation, eulerRotation);
	model = glm::rotate(model, eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	if (currentState == ATTACKING) {
		btVector3 playerPos(player->getPosition().x, player->getPosition().y, player->getPosition().z);
		float toPlayerAngle = (atan2(playerPos.z() - pos.z(), playerPos.x() - pos.x())); // RADIANS
		model = glm::rotate(model, std::atan2(std::cos(toPlayerAngle), std::sin(toPlayerAngle)), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		model = glm::rotate(model, std::atan2(std::cos(angle), std::sin(angle)), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	model = glm::rotate(model, eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	if (this->name == "boss") { model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05)); }
	else
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));	// It's a bit too big for our scene, so scale it down

	glUniform1i(glGetUniformLocation(shader, "animated"), 1); //zero is no animations
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void NonPC::setAttack(double atk) { this->attack = atk; }
double NonPC::getAttack() { return this->attack; }
void NonPC::modifyHealth(double newHp) { this->health += newHp; }
double NonPC::getRange() { return range; }
void NonPC::setRange(double rng) { this->range = rng; }
double NonPC::getHealth() { return health; }
void NonPC::setAttackSpeed(double atkspd) {
	this->attackSpeed = atkspd;
}
double NonPC::getAttackSpeed() {
	return this->attackSpeed;
}
void NonPC::setName(string _name) { name = _name; }
string NonPC::getName() { return name; }

void NonPC::setState(npcState newState) { this->currentState = newState; }
npcState NonPC::getState() { return this->currentState; }
