#ifndef MESHMANAGER
#define MESHMANAGER

#include <GL/glew.h>
#include "ShaderManager.h"



namespace MeshManager {

	struct lightStruct {
		GLfloat ambient[4];
		GLfloat diffuse[4];
		GLfloat specular[4];
		GLfloat position[4];
	};

	struct materialStruct {
		GLfloat ambient[4];
		GLfloat diffuse[4];
		GLfloat specular[4];
		GLfloat shininess;
	};

	// Some methods for creating meshes
	// ... including one for dealing with indexed meshes
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, const GLfloat* normals,
		const GLfloat* texcoords, const GLuint indexCount, const GLuint* indices);

	// these three create mesh functions simply provide more basic access to the full version
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, const GLfloat* normals,
		const GLfloat* texcoords);
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices);
	GLuint createColourMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours);

	void setUniformMatrix4fv(const GLuint program, const char* uniformName, const GLfloat *data);
	void setMatrices(const GLuint program, const GLfloat *proj, const GLfloat *mv, const GLfloat *mvp);
	void setLight(const GLuint program, const lightStruct light);
	void setLightPos(const GLuint program, const GLfloat *lightPos);
	void setMaterial(const GLuint program, const materialStruct material);

	void drawMesh(const GLuint mesh, const GLuint numVerts, const GLuint primitive);
	void drawIndexedMesh(const GLuint mesh, const GLuint indexCount, const GLuint primitive);

	void updateMesh(const GLuint mesh, const unsigned int bufferType, const GLfloat *data, const GLuint size);
}

#endif