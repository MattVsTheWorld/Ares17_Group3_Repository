#include "Object.h"

Object::Object(){
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

GLuint Object::object_getIndex() {
	return meshIndexCount;
}
GLuint Object::object_getTexture() {
	return texture;
}
GLuint Object::object_getMesh() {
	return meshObject;
}