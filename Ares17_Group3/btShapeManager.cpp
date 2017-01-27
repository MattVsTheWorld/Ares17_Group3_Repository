#include "btShapeManager.h"
/*
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
	//world->addRigidBody(body);
	bodies.push_back(body);

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
	//world->addRigidBody(body);
	bodies.push_back(body);
	return body;
}

void btShapeManager::renderSphere(btRigidBody* sphere, glm::mat4 proj, Model *modelData, MeshManager::materialStruct material,
	GLuint modelProgram, ) {

	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE) //cout << "Wrong collision shape ";	
		return;

	glUseProgram(modelProgram);
	mvStack.push(mvStack.top());// push modelview to stack

								//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
								// Draw the loaded model
								//MeshManager::setLight(modelProgram, testLight);
	MeshManager::setLight(shaderProgram, testLight);
	MeshManager::setMaterial(modelProgram, material);

	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));

	float r = ((btSphereShape*)sphere->getCollisionShape())->getRadius();

	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);
	glm::mat4 model;
	t.getOpenGLMatrix(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defaultTexture);

	model = glm::scale(model, glm::vec3(r, r, r));
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(modelProgram);

	glBindTexture(GL_TEXTURE_2D, 0);
	mvStack.pop();
}

// Just move it after
void btShapeManager::renderBox(btRigidBody* box, glm::mat4 proj, Model *modelData, MeshManager::materialStruct material) {

	if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE) 			//cout << "Wrong collision shape";
		return;

	glUseProgram(modelProgram);
	//MeshManager::setLight(shaderProgram, testLight);
	//MeshManager::setMaterial(shaderProgram, defaultMaterial);
	mvStack.push(mvStack.top());// push modelview to stack
	MeshManager::setLight(modelProgram, testLight);
	MeshManager::setMaterial(modelProgram, material);
	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));

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

	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
	modelData->Draw(modelProgram);

	glBindTexture(GL_TEXTURE_2D, 0);
	mvStack.pop();
}
*/