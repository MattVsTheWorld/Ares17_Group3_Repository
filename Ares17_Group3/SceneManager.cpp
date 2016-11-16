#include "SceneManager.h"
#include "Model.h"
#include "Shader.h"
using namespace std;

// this class still needs a lot of work
namespace SceneManager {
	Object *testCube;
	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;
	hudManager *h_manager;
	Skybox *skybox;

	float SCREENWIDTH = 800.0f;
	float SCREENHEIGHT = 600.0f;

	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	// Setup and compile our shaders
	Shader shader("modelLoading.vert", "modelLoading.frag");

	// Load models
	Model ourModel("Nanosuit/nanosuit.obj");

	typedef stack<glm::mat4> mvstack;
	mvstack mvStack;

	GLfloat camRotation = 0.0f;
	GLfloat camy = 0.0f;

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



	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(camRotation*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(camRotation*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(camRotation*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(camRotation*DEG_TO_RADIAN));
	}

	void lockCamera()
	{
		if (camy>70)
			camy = 70;
		if (camy<-70)
			camy = -70;
		if (camRotation<0.0)
			camRotation += 360.0;
		if (camRotation>360.0)
			camRotation -= 360;
	}

	void controls(SDL_Event event, SDL_Window * window) {

		if (event.type == SDL_MOUSEMOTION)
		{
			int MidX = SCREENWIDTH / 2;
			int MidY = SCREENHEIGHT / 2;
			SDL_ShowCursor(SDL_DISABLE);
			int tmpx, tmpy;
			SDL_GetMouseState(&tmpx, &tmpy);
			camRotation -= 0.1*(MidX - tmpx); //for y
			camy -= 0.1*(MidY - tmpy)/10; //for x
			lockCamera();

			//rotate the camera (move everything in the opposit direction)
			glRotatef(-camy, 1.0, 0.0, 0.0);       
			glRotatef(-camRotation, 0.0, 1.0, 0.0);
			SDL_WarpMouseInWindow(window, MidX, MidY);
		}

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, camRotation, 0.1f);
		else if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, camRotation, -0.1f);
		if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, camRotation, -0.1f);
		else if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, camRotation, 0.1f);
		if (keys[SDL_SCANCODE_R]) eye.y += 0.1;
		else if (keys[SDL_SCANCODE_F]) eye.y -= 0.1;

		if (keys[SDL_SCANCODE_COMMA]) camRotation -= 1.0f;
		else if (keys[SDL_SCANCODE_PERIOD]) camRotation += 1.0f;

		if (keys[SDL_SCANCODE_O]) camy += 0.05; // move camera downwards (because of how the controls are set)
		else if (keys[SDL_SCANCODE_P]) camy -= 0.05; // move camera upwards

		if (keys[SDL_SCANCODE_1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}
		if (keys[SDL_SCANCODE_2]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}
		if (keys[SDL_SCANCODE_ESCAPE]) {
			exit(0);
		}
	}


	void init(void) {
		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		//modelProgram = ShaderManager::initShaders("modelLoading.vert", "modelLoading.frag");
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, testMaterial);
		testCube = new Object();
		h_manager = new hudManager();
		skybox = new Skybox(testTexFiles);
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

	void renderObject(glm::mat4 proj) {
		shader.Use();
	//	MeshManager::setLight(shader.Program, testLight);
		mvStack.push(mvStack.top());// push modelview to stack
		MeshManager::setUniformMatrix4fv(shader.Program, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(shader.Program, "view", glm::value_ptr(mvStack.top()));
		mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(shader);
		//mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
		//glBindTexture(GL_TEXTURE_2D, testCube->object_getTexture());
		
		MeshManager::setMaterial(shader.Program, testMaterial);
		mvStack.pop();
	}

	void update(SDL_Event event, SDL_Window * window) {
		controls(event, window);
	}

	void camera() {
		at = moveForward(eye, camRotation, 1.0f);
		at.y -= camy;
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
		projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), SCREENWIDTH / SCREENHEIGHT, 1.0f, 100.0f);

		mvStack = skybox->renderSkybox(projection, mvStack, testCube->object_getMesh(), testCube->object_getIndex());

		renderTestCube(projection);
		renderObject(projection);

		// h_manager->renderFPS(texturedProgram, testLight, glm::mat4(1.0), testCube->object_getMesh(), testCube->object_getIndex(), fps);

		SDL_GL_SwapWindow(window); // swap buffers
	}
}