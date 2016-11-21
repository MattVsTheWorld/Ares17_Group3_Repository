#include "Object.h"

// probably get rid of this constructor
Object::Object(glm::vec3 pos, glm::vec3 scal, glm::vec3 rot) {
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap("wall.bmp"); // load a texture
	meshObject = MeshManager::createMesh(verts.size() / 3, verts.data(), nullptr,
		norms.data(), tex_coords.data(), size, indices.data()); // create a mesh
	position = pos;
	scale = scal;
	rotation = rot;
}

Object::Object(glm::vec3 pos, glm::vec3 scal, glm::vec3 rot, char *texturePath) {
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap(texturePath); // load a texture
	meshObject = MeshManager::createMesh(verts.size() / 3, verts.data(), nullptr,
		norms.data(), tex_coords.data(), size, indices.data()); // create a mesh
	position = pos;
	scale = scal;
	rotation = rot;
}

Object::~Object() {
//	delete this;
}

std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
	MeshManager::lightStruct light, MeshManager::materialStruct material, float rot) {

	glUseProgram(shader);
	MeshManager::setLight(shader, light);
	mvStack.push(mvStack.top());// push modelview to stack
	if (position != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::translate(mvStack.top(), position);
	if (rotation != glm::vec3(0.0f, 0.0f, 0.0f)){
		rotationAngle = rot;
		mvStack.top() = glm::rotate(mvStack.top(), rotationAngle, rotation);
	}
	if (scale != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::scale(mvStack.top(), scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	MeshManager::setUniformMatrix4fv(shader, "modelview", glm::value_ptr(mvStack.top()));
	MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(projection));
	MeshManager::setMaterial(shader, material);
	MeshManager::drawIndexedMesh(meshObject, meshIndexCount, GL_TRIANGLES);
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
	position = newPos;
}