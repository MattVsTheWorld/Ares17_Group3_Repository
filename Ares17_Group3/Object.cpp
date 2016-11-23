#include "Object.h"

// probably get rid of this constructor
Object::Object(transformation_Matrices transformation) {
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap("wall.bmp"); // load a texture
	meshObject = MeshManager::createMesh(verts.size() / 3, verts.data(), nullptr,
		norms.data(), tex_coords.data(), size, indices.data()); // create a mesh
	trans_m.position = transformation.position;
	trans_m.scale = transformation.scale;
	trans_m.rotation = transformation.rotation;
//	scale = scal;
//	rotation = rot;
}

Object::Object(transformation_Matrices transformation, char *texturePath) {
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap(texturePath); // load a texture
	meshObject = MeshManager::createMesh(verts.size() / 3, verts.data(), nullptr,
		norms.data(), tex_coords.data(), size, indices.data()); // create a mesh
	trans_m.position = transformation.position;
	trans_m.scale = transformation.scale;
	trans_m.rotation = transformation.rotation;
}

Object::~Object() {
//	delete this;
}

std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
	MeshManager::lightStruct light, MeshManager::materialStruct material, float rot) {

	glUseProgram(shader);
	MeshManager::setLight(shader, light);
	mvStack.push(mvStack.top());// push modelview to stack
	if (trans_m.position != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::translate(mvStack.top(), trans_m.position);
	if (trans_m.rotation != glm::vec3(0.0f, 0.0f, 0.0f)){
		rotationAngle = rot;
		mvStack.top() = glm::rotate(mvStack.top(), rotationAngle, trans_m.rotation);
	}
	if (trans_m.scale != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::scale(mvStack.top(), trans_m.scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	MeshManager::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(mvStack.top()));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(projection));
	MeshManager::setMaterial(shader, material);
	MeshManager::drawIndexedMesh(meshObject, meshIndexCount, GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);
	mvStack.pop();
	return mvStack;
}


GLuint Object::object_getIndex() {
	return meshIndexCount;
}
GLuint Object::object_getTexture() {
	return texture;
}
GLuint Object::object_getMesh() {
	return meshObject;
}

void Object::setPosition(glm::vec3 newPos) {
	trans_m.position = newPos;
}

glm::vec3 Object::getPosition() {
	return trans_m.position;
}

void Object::setVelocity(glm::vec3 newVel) {
	phys_p.velocity = newVel;
}

glm::vec3 Object::getVelocity() {
	return phys_p.velocity;
}

glm::vec3 Object::getScale() {
	return trans_m.scale;
}

float Object::getAngle() {
	return angleAtShot;
}
void Object::setAngle(float angle) {
	angleAtShot = angle;
}

////EXPERIMENTING
void Object::setAt(glm::vec3 newAt) {
	at = newAt;
}
glm::vec3 Object::getAt() {
	return at;
}

void Object::setInitialPosition(glm::vec3 newPos) {
	initialPos = newPos;
}

glm::vec3 Object::getInitialPosition() {
	return initialPos;
}
