#include "btShapeManager.h"

btShapeManager::btShapeManager() {
	btSettings.collisionConfig = new btDefaultCollisionConfiguration();
	btSettings.dispatcher = new btCollisionDispatcher(btSettings.collisionConfig); //base algorithm good (?)
	btSettings.broadphase = new btDbvtBroadphase(); //divide space into different spaces // can use a more performancy ones // axis sweep?
	btSettings.solver = new btSequentialImpulseConstraintSolver(); // can use OpenCL, multithreading (?)
	btSettings.world = new btDiscreteDynamicsWorld(btSettings.dispatcher, btSettings.broadphase, btSettings.solver, btSettings.collisionConfig);
	btSettings.world->setGravity(btVector3(0, -10, 0)); // x y z as usual

	//modelProgram = shader;
	//light = _light;
	//test
	defaultTexture = loadBitmap::loadBitmap("wall.bmp");
}

void btShapeManager::update() {
	btSettings.world->stepSimulation(1 / 60.0);
}

btRigidBody* btShapeManager::addBox(float width, float height, float depth, float x, float y, float z, float mass)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btBoxShape* box = new btBoxShape(btVector3(width / 2, height / 2, depth / 2));
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		box->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);

	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);

	btRigidBody* body = new btRigidBody(info);
	btSettings.world->addRigidBody(body);
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
	btSettings.world->addRigidBody(body);
	//bodies.push_back(body);
	return body;
}

void btShapeManager::renderSphere(btRigidBody* sphere, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader) {

	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE) //cout << "Wrong collision shape ";	
		return;

//	glUseProgram(modelProgram);
	//mvStack.push(mvStack.top());// push modelview to stack

	//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	// Draw the loaded model
	//MeshManager::setLight(modelProgram, testLight);
//	MeshManager::setLight(modelProgram, light);
//	MeshManager::setMaterial(modelProgram, material);

//	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
//	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(view));

	float r = ((btSphereShape*)sphere->getCollisionShape())->getRadius();

	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);
	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);

	model = glm::scale(model, glm::vec3(r, r, r));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);
	//	mvStack.pop();
}

void btShapeManager::renderCapsule(btRigidBody* capsule, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader) {

	if (capsule->getCollisionShape()->getShapeType() != CAPSULE_SHAPE_PROXYTYPE) //cout << "Wrong collision shape ";	
		return;

//	glUseProgram(modelProgram);
	//mvStack.push(mvStack.top());// push modelview to stack

	//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	// Draw the loaded model
	//MeshManager::setLight(modelProgram, testLight);
//	MeshManager::setLight(modelProgram, light);
//	MeshManager::setMaterial(modelProgram, material);

//	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
//	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(view));

	float r = ((btCapsuleShape*)capsule->getCollisionShape())->getRadius();
	float h = ((btCapsuleShape*)capsule->getCollisionShape())->getHalfHeight()*2;

	btTransform t;
	capsule->getMotionState()->getWorldTransform(t);
	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);

	model = glm::scale(model, glm::vec3(r, h, r));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);
	//	mvStack.pop();
}

// Just move it after
void btShapeManager::renderBox(btRigidBody* box, glm::mat4 view, glm::mat4 proj, Model *modelData, GLuint shader) {

	if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE) 			//cout << "Wrong collision shape";
		return;

//	glUseProgram(modelProgram);
	//MeshManager::setLight(shaderProgram, testLight);
	//MeshManager::setMaterial(shaderProgram, defaultMaterial);
	//	mvStack.push(mvStack.top());// push modelview to stack
//	MeshManager::setLight(modelProgram, light);
//	MeshManager::setMaterial(modelProgram, material);
//	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
//	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(view));

	btVector3 extent = ((btBoxShape*)box->getCollisionShape())->getHalfExtentsWithMargin();
	btTransform t;
	box->getMotionState()->getWorldTransform(t);


	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	model = glm::scale(model, glm::vec3(extent.x(), extent.y(), extent.z())); //DEFINITELY goes after

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);
	//MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));		
	//MeshManager::drawIndexedMesh(testCube->object_getMesh(), testCube->object_getIndex(), GL_TRIANGLES);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(shader);

	glBindTexture(GL_TEXTURE_2D, 0);
	//	mvStack.pop();
}

void btShapeManager::addToWorld(btRigidBody* body) {
	btSettings.world->addRigidBody(body);
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