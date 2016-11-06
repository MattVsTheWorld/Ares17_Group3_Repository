#include "SceneManager.h"

using namespace std;

namespace SceneManager {
	Object *testCube;
	GLuint shaderProgram;
	GLuint texturedProgram;
	hudManager *h_manager;
	
	GLfloat camRotation = 0.0f;

	glm::vec3 eye(0.0f, 1.0f, 0.0f);
	glm::vec3 at(0.0f, 1.0f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	MeshManager::lightStruct testLight = {
		{ 0.6f, 0.4f, 0.6f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ 0.0f, 5.0f, 0.0f, 1.0f }  // position
	};

	MeshManager::materialStruct testMaterial = {
		{ 0.6f, 0.4f, 0.2f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};

	stack<glm::mat4> mvStack;

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(camRotation*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(camRotation*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(camRotation*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(camRotation*DEG_TO_RADIAN));
	}


	void controls() {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, camRotation, 0.1f);
		if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, camRotation, -0.1f);
		if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, camRotation, -0.1f);
		if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, camRotation, 0.1f);
		if (keys[SDL_SCANCODE_R]) eye.y += 0.1;
		if (keys[SDL_SCANCODE_F]) eye.y -= 0.1;

		if (keys[SDL_SCANCODE_COMMA]) camRotation -= 1.0f;
		if (keys[SDL_SCANCODE_PERIOD]) camRotation += 1.0f;

		if (keys[SDL_SCANCODE_1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}
		if (keys[SDL_SCANCODE_2]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}
	}


	void init(void) {
		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, testMaterial);
		testCube = new Object();
		h_manager = new hudManager();
	}

	void renderTestCube(glm::mat4 proj) {
		glUseProgram(shaderProgram);
		MeshManager::setLight(shaderProgram, testLight);
		mvStack.push(mvStack.top());// push modelview to stack
		mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
		glBindTexture(GL_TEXTURE_2D, testCube->object_getTexture());
		MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(proj));
		MeshManager::setMaterial(shaderProgram, testMaterial);
		MeshManager::drawIndexedMesh(testCube->object_getMesh(), testCube->object_getIndex(), GL_TRIANGLES);
		mvStack.pop();
	}

	void update(void) {
		controls();
	}

	void camera() {
		at = moveForward(eye, camRotation, 1.0f);
		mvStack.top() = glm::lookAt(eye, at, up);
	}


	void draw(SDL_Window * window, float fps) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window
		glEnable(GL_CULL_FACE);
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glm::mat4 projection(1.0);
		GLfloat scale(1.0f); // just to allow easy scaling of complete scene
		glm::mat4 modelview(1.0); // set base position for scene
		mvStack.push(modelview);

		camera();

		projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 100.0f);
	
		renderTestCube(projection);
	
		h_manager->renderFPS(texturedProgram, testLight, glm::mat4 (1.0), testCube->object_getMesh(), testCube->object_getIndex(), fps);
	
		SDL_GL_SwapWindow(window); // swap buffers
	}
}