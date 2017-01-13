#include "MeshManager.h"
#include <map>

/*
Based on rt3d.cpp from Real Time 3d graphics module
*/

namespace MeshManager {

	// struct vaoBuffers will be used inside the library
	// clients should not need to know about this data structure
	struct vaoBuffers {
		GLuint vertex_buffer;
		GLuint colour_buffer;
		GLuint normal_buffer;
		GLuint texcoord_buffer;
		GLuint index_buffer;
	};

	static std::map<GLuint, GLuint *> vertexArrayMap;

	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours,
		const GLfloat* normals, const GLfloat* texcoords, const GLuint indexCount, const GLuint* indices) {
		GLuint VAO;
		// generate and set up a VAO for the mesh
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint *pMeshBuffers = new GLuint[5];


		if (vertices == nullptr) {
			// cant create a mesh without vertices... oops
			ShaderManager::exitFatalError("Attempt to create a mesh with no vertices");
		}

		// generate and set up the VBOs for the data
		GLuint VBO;
		glGenBuffers(1, &VBO);

		// VBO for vertex data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 3 * numVerts * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)LOCATION_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(LOCATION_VERTEX);
		pMeshBuffers[LOCATION_VERTEX] = VBO;


		// VBO for colour data
		if (colours != nullptr) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 3 * numVerts * sizeof(GLfloat), colours, GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)LOCATION_COLOUR, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(LOCATION_COLOUR);
			pMeshBuffers[LOCATION_COLOUR] = VBO;
		}

		// VBO for normal data
		if (normals != nullptr) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 3 * numVerts * sizeof(GLfloat), normals, GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(LOCATION_NORMAL);
			pMeshBuffers[LOCATION_NORMAL] = VBO;
		}

		// VBO for tex-coord data
		if (texcoords != nullptr) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 2 * numVerts * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(LOCATION_TEXCOORD);
			pMeshBuffers[LOCATION_TEXCOORD] = VBO;
		}

		if (indices != nullptr && indexCount > 0) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
			pMeshBuffers[LOCATION_INDEX] = VBO;
		}
		// unbind vertex array
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// return the identifier needed to draw this mesh

		vertexArrayMap.insert(std::pair<GLuint, GLuint *>(VAO, pMeshBuffers));

		return VAO;
	}

	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours,
		const GLfloat* normals, const GLfloat* texcoords) {
		return createMesh(numVerts, vertices, colours, normals, texcoords, 0, nullptr);
	}

	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices) {
		return createMesh(numVerts, vertices, nullptr, nullptr, nullptr);
	}

	GLuint createColourMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours) {
		return createMesh(numVerts, vertices, colours, nullptr, nullptr);
	}

	void setUniformMatrix4fv(const GLuint program, const char* uniformName, const GLfloat *data) {
		int uniformIndex = glGetUniformLocation(program, uniformName);
		glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, data);
	}

	// set matrices untested... likely to change - not totally happy with this for now.
	void setMatrices(const GLuint program, const GLfloat *proj, const GLfloat *mv, const GLfloat *mvp) {
		int uniformIndex = glGetUniformLocation(program, "modelview");
		glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, mv);
		uniformIndex = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, proj);
		uniformIndex = glGetUniformLocation(program, "MVP");
		glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, mvp);
		uniformIndex = glGetUniformLocation(program, "normalmatrix");
	}

	void setLightPos(const GLuint program, const GLfloat *lightPos) {
		int uniformIndex = glGetUniformLocation(program, "lightPosition");
		glUniform4fv(uniformIndex, 1, lightPos);
	}

	void setProjection(const GLuint program, const GLfloat *data) {
		int uniformIndex = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, data);
	}

	void setLight(const GLuint program, const lightStruct light) {
		// pass in light data to shader
		int uniformIndex = glGetUniformLocation(program, "light.ambient");
		glUniform4fv(uniformIndex, 1, light.ambient);
		uniformIndex = glGetUniformLocation(program, "light.diffuse");
		glUniform4fv(uniformIndex, 1, light.diffuse);
		uniformIndex = glGetUniformLocation(program, "light.specular");
		glUniform4fv(uniformIndex, 1, light.specular);
		uniformIndex = glGetUniformLocation(program, "lightPosition");
		glUniform4fv(uniformIndex, 1, light.position);
	}


	void setMaterial(const GLuint program, const materialStruct material) {
		// pass in material data to shader 
		int uniformIndex = glGetUniformLocation(program, "material.ambient");
		glUniform4fv(uniformIndex, 1, material.ambient);
		uniformIndex = glGetUniformLocation(program, "material.diffuse");
		glUniform4fv(uniformIndex, 1, material.diffuse);
		uniformIndex = glGetUniformLocation(program, "material.specular");
		glUniform4fv(uniformIndex, 1, material.specular);
		uniformIndex = glGetUniformLocation(program, "material.shininess");
		glUniform1f(uniformIndex, material.shininess);
	}

	void drawMesh(const GLuint mesh, const GLuint numVerts, const GLuint primitive) {
		glBindVertexArray(mesh);	// Bind mesh VAO
		glDrawArrays(primitive, 0, numVerts);	// draw first vertex array object
		glBindVertexArray(0);
	}


	void drawIndexedMesh(const GLuint mesh, const GLuint indexCount, const GLuint primitive) {
		glBindVertexArray(mesh);	// Bind mesh VAO
		glDrawElements(primitive, indexCount, GL_UNSIGNED_INT, 0);	// draw VAO 
		glBindVertexArray(0);
	}


	void updateMesh(const GLuint mesh, const unsigned int bufferType, const GLfloat *data, const GLuint size) {
		GLuint * pMeshBuffers = vertexArrayMap[mesh];
		glBindVertexArray(mesh);

		// Delete the old buffer data
		glDeleteBuffers(1, &pMeshBuffers[bufferType]);

		// generate and set up the VBOs for the new data
		GLuint VBO;
		glGenBuffers(1, &VBO);
		// VBO for the data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), data, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)bufferType, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(bufferType);
		pMeshBuffers[LOCATION_VERTEX] = VBO;

		glBindVertexArray(0);

	}
}