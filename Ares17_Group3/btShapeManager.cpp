#include "btShapeManager.h"

btShapeManager::btShapeManager() {
	btSettings.collisionConfig = new btDefaultCollisionConfiguration();
	btSettings.dispatcher = new btCollisionDispatcher(btSettings.collisionConfig); //base algorithm good (?)
	btSettings.broadphase = new btDbvtBroadphase(); //divide space into different spaces // can use a more performancy ones // axis sweep?
	btSettings.solver = new btSequentialImpulseConstraintSolver(); // can use OpenCL, multithreading (?)
	btSettings.world = new btDiscreteDynamicsWorld(btSettings.dispatcher, btSettings.broadphase, btSettings.solver, btSettings.collisionConfig);
	btSettings.world->setGravity(btVector3(0, GRAVITY, 0)); // x y z as usual
}

void btShapeManager::update() {
	btSettings.world->stepSimulation(1 / 60.0);
}

btRigidBody* btShapeManager::addBox(float width, float height, float depth, float x, float y, float z, float mass)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btBoxShape* box = new btBoxShape(btVector3(width, height, depth));
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		box->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);

	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);

	btRigidBody* body = new btRigidBody(info);
	btSettings.world->addRigidBody(body, COL_DEFAULT, COLLIDE_ALL);
	//bodies.push_back(body);

	return body;
}

btRigidBody* btShapeManager::addSphere(float rad, float x, float y, float z, float mass)
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
	btSettings.world->addRigidBody(body, COL_DEFAULT, COLLIDE_ALL);
	//bodies.push_back(body);
	return body;
}

btRigidBody* btShapeManager::addCapsule(float rad, float height, float x, float y, float z, float mass) {

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
	btSettings.world->addRigidBody(body, COL_DEFAULT, COLLIDE_ALL);

	return body;
}

void btShapeManager::renderSphere(btRigidBody* sphere, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture) {

	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE) //cout << "Wrong collision shape ";	
		return;

	float r = ((btSphereShape*)sphere->getCollisionShape())->getRadius();

	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);
	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0); // ? not working ?
	glBindTexture(GL_TEXTURE_2D, texture);

	model = glm::scale(model, glm::vec3(r, r, r));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void btShapeManager::renderCapsule(btRigidBody* capsule, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture) {

	if (capsule->getCollisionShape()->getShapeType() != CAPSULE_SHAPE_PROXYTYPE) //cout << "Wrong collision shape ";	
		return;

	float r = ((btCapsuleShape*)capsule->getCollisionShape())->getRadius();
	float h = ((btCapsuleShape*)capsule->getCollisionShape())->getHalfHeight()*2;

	btTransform t;
	capsule->getMotionState()->getWorldTransform(t);
	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	model = glm::scale(model, glm::vec3(r, h, r));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void btShapeManager::renderBox(btRigidBody* box, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader, GLuint texture) {

	if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE) 			//cout << "Wrong collision shape";
		return;

	btVector3 extent = ((btBoxShape*)box->getCollisionShape())->getHalfExtentsWithMargin();
	btTransform t;
	box->getMotionState()->getWorldTransform(t);

	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));
	model = glm::scale(model, glm::vec3(extent.x(), extent.y(), extent.z())); //DEFINITELY goes after

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void btShapeManager::addToWorld(btRigidBody* body) {
	btSettings.world->addRigidBody(body);
}

void btShapeManager::addToWorld(btRigidBody* body, collisiontype COLL_TYPE, int collidesWith) {
	btSettings.world->addRigidBody(body, COLL_TYPE, collidesWith);
}


void btShapeManager::addGhostToWorld(btPairCachingGhostObject* ghost) {
	btSettings.world->addCollisionObject(ghost, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter); // ?
	btSettings.world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

void btShapeManager::addGhostToWorld(btPairCachingGhostObject* ghost, collisiontype COLL_TYPE, int collidesWith) {
	btSettings.world->addCollisionObject(ghost, COLL_TYPE, collidesWith); // ?
	

	// world->addCollisionObject(ghostobject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	btSettings.world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

btBroadphasePair* btShapeManager::findWorldPair(const btBroadphasePair &pair) {
	return btSettings.world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
}

void btShapeManager::removeObject(btRigidBody* body) {
	btSettings.world->removeCollisionObject(body);
}
void btShapeManager::removeObject(btPairCachingGhostObject* ghost) {
	btSettings.world->removeCollisionObject(ghost);
}

// Create plane + info on bullet
/*
btTransform t; // orientation and position // quaternion :))))) """SIMPLE VARIABLE"""
t.setIdentity(); // 0 position, 0 rotation
t.setOrigin(btVector3(0, 0, 0));
btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0,1,0),0); // 1st == normal (0,1,0) -> pointing upwards // second == distance from origin = 0 (AA) //infinite in every direction
//motion state, although position for this is already set
btMotionState* motion = new btDefaultMotionState(t);
// mass = 0 == static, mass > 0 == dinamic ( < 0 no sense)
btRigidBody::btRigidBodyConstructionInfo info(0,motion,plane); // no need to dynamically allocate
// for dinamic bodies will have to add inertia (?)
btRigidBody* body = new btRigidBody(info);
world->addRigidBody(body);
bodies.push_back(body);
*/

/*
void renderPlane(btRigidBody* plane, glm::mat4 projection)
{
if (plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
{
return;
cout << "Wrong collision shape (?)";
}
//glColor3f(0.8, 0.8, 0.8);
glUseProgram(shaderProgram);
MeshManager::setLight(shaderProgram, testLight);
MeshManager::setMaterial(shaderProgram, defaultMaterial);
mvStack.push(mvStack.top());// push modelview to stack

btTransform t;
plane->getMotionState()->getWorldTransform(t);

t.getOpenGLMatrix(glm::value_ptr(mvStack.top()));

/*
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, testCubes[0]->object_getTexture());
mvStack.top() = glm::scale(mvStack.top(), glm::vec3(10, 0.5, 10));
MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
//cout << "GASGHDJ";

MeshManager::drawIndexedMesh(testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), GL_TRIANGLES);
glBindTexture(GL_TEXTURE_2D, 0);

mvStack.pop();

} */