#include "Object.h"

Object::Object(){
	std::cout << "++ load object debug message, once\n";
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap("wall.bmp"); // load a texture
	meshObject = MeshManager::createMesh(verts.size() / 3, verts.data(), nullptr,
		norms.data(), tex_coords.data(), size, indices.data()); // create a mesh
}

Object::~Object() {
//	delete this;
}

std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
	MeshManager::lightStruct light, MeshManager::materialStruct material, 
	glm::vec3 transVec, glm::vec3 scaleVec, glm::vec3 rotateVec, float angle) {

	glUseProgram(shader);
	MeshManager::setLight(shader, light);
	MeshManager::setMaterial(shader, material);
	mvStack.push(mvStack.top());// push modelview to stack
//	if (transVec != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::translate(mvStack.top(), transVec);
	if (rotateVec != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::rotate(mvStack.top(), angle, rotateVec);
//	if (scaleVec != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::scale(mvStack.top(), scaleVec);
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