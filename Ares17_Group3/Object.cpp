#include "Object.h"

// probably get rid of this constructor
Object::Object(transformation_Matrices transformation, object_Properties obj_p) {
	GLuint size = obj_p.indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap("wall.bmp"); // load a texture
	meshObject = MeshManager::createMesh(obj_p.verts.size() / 3, obj_p.verts.data(), nullptr,
		obj_p.norms.data(), obj_p.tex_coords.data(), size, obj_p.indices.data()); // create a mesh
	trans_m.position = transformation.position;
	trans_m.scale = transformation.scale;
	trans_m.pitch = transformation.pitch;
	trans_m.yaw = transformation.yaw;
	trans_m.roll = transformation.roll;
//	scale = scal;
}


Object::Object(transformation_Matrices transformation, char *texturePath, object_Properties obj_p) {
	GLuint size = obj_p.indices.size();
	meshIndexCount = size;
	texture = loadBitmap::loadBitmap(texturePath); // load a texture
	meshObject = MeshManager::createMesh(obj_p.verts.size() / 3, obj_p.verts.data(), nullptr,
		obj_p.norms.data(), obj_p.tex_coords.data(), size, obj_p.indices.data()); // create a mesh
	trans_m.position = transformation.position;
	trans_m.scale = transformation.scale;
	trans_m.pitch = transformation.pitch;
	trans_m.yaw = transformation.yaw;
	trans_m.roll = transformation.roll;
}

Object::~Object() {
//	delete this;
}


object_Properties Object::initializeObject(char *objectPath) {
	vector<GLfloat> verts; // contains vertices of loaded object
	vector<GLfloat> norms; // contains normal of loaded object
	vector<GLfloat> tex_coords; // contains texture coordinates of loaded object
	vector<GLuint> indices; // contains indices of loaded object
	rt3d::loadObj(objectPath, verts, norms, tex_coords, indices);

	return{ verts, norms, tex_coords, indices };
}

std::stack<glm::mat4> Object::renderObject(glm::mat4 projection, std::stack<glm::mat4> mvStack, GLuint shader,
	MeshManager::lightStruct light, MeshManager::materialStruct material, float pitch, float yaw, float roll) {

	glUseProgram(shader);
	MeshManager::setLight(shader, light);
	mvStack.push(mvStack.top());// push modelview to stack
	if (trans_m.position != glm::vec3(0.0f, 0.0f, 0.0f))
		mvStack.top() = glm::translate(mvStack.top(), trans_m.position);
	if (trans_m.pitch != glm::vec3(0.0f, 0.0f, 0.0f)) {
		pitchAngle = pitch;
		mvStack.top() = glm::rotate(mvStack.top(), pitchAngle, trans_m.pitch);
	}
	if (trans_m.yaw != glm::vec3(0.0f, 0.0f, 0.0f)){
		yawAngle = yaw;
		mvStack.top() = glm::rotate(mvStack.top(), yawAngle, trans_m.yaw);
	}
	if (trans_m.roll != glm::vec3(0.0f, 0.0f, 0.0f)) {
		rollAngle = roll;
		mvStack.top() = glm::rotate(mvStack.top(), rollAngle, trans_m.roll);
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
