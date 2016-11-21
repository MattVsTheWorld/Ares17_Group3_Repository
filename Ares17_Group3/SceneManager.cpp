#include "SceneManager.h"
#include "Model.h"
//#include "Shader.h"
using namespace std;

// this class still needs a lot of work
namespace SceneManager {
	Object *testCube;
	Object *testCube2;
	Object *testCube3;
	Object *testCube4;
	float theta = 0.0f;
	float movement = 0.05;
	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;
	hudManager *h_manager;
	Skybox *skybox;
	Physics *physicsManager;

	float SCREENWIDTH = 800.0f;
	float SCREENHEIGHT = 600.0f;

	glm::vec3 transTest = glm::vec3(-10.0f, -0.1f, -10.0f);		
	glm::vec3 scaleTest = glm::vec3(20.0f, 0.1f, 20.0f);
	glm::vec3 nullTest = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 testMove = glm::vec3(0.0, 2.0, -2.0);

	transformation_Matrices testTransformation = {
		transTest, scaleTest, nullTest
	};

	float angleTest;

	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	// Setup and compile our shaders
//	Shader *shader;

	// Load models
	Model *ourModel;
	Model *ourModel2;


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

	MeshManager::materialStruct greenMaterial = {
		{ 0.6f, 0.4f, 0.2f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};

	MeshManager::materialStruct defaultMaterial = {
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // ambient
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // diffuse
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // specular
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
		if (camy > 70)
			camy = 70;
		if (camy < -70)
			camy = -70;
		if (camRotation < 0.0)
			camRotation += 360.0;
		if (camRotation > 360.0)
			camRotation -= 360;
	}

	void controls(SDL_Window * window) {
		int MidX = SCREENWIDTH / 2;
		int MidY = SCREENHEIGHT / 2;

		SDL_ShowCursor(SDL_DISABLE);
		int tmpx, tmpy;
		SDL_GetMouseState(&tmpx, &tmpy);
		camRotation -= 0.1*(MidX - tmpx); //for y
		camy -= 0.1*(MidY - tmpy) / 10; //for x
		lockCamera();

		//rotate the camera (move everything in the opposit direction)
		glRotatef(-camy, 1.0, 0.0, 0.0);
		glRotatef(-camRotation, 0.0, 1.0, 0.0);
		SDL_WarpMouseInWindow(window, MidX, MidY);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, camRotation, 0.1f);
		else if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, camRotation, -0.1f);
		if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, camRotation, -0.1f);
		else if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, camRotation, 0.1f);
		if (keys[SDL_SCANCODE_R]) eye.y += 0.1;
		else if (keys[SDL_SCANCODE_F]) eye.y -= 0.1;

	//	if (keys[SDL_SCANCODE_COMMA]) camRotation -= 1.0f;
	//	else if (keys[SDL_SCANCODE_PERIOD]) camRotation += 1.0f;s

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

	void initObjects() {
		testCube = new Object(testTransformation);
		transformation_Matrices test2 = { testMove, glm::vec3(0.5, 0.5, 0.5), nullTest };
	//	transformation_Matrices(testMove, glm::vec3(0.5, 0.5, 0.5), nullTest)
		testCube2 = new Object(test2, "studdedmetal.bmp");
		transformation_Matrices test3 = { glm::vec3(-3.0, 2.0, 0.0), glm::vec3(0.5, 1.5, 0.5), glm::vec3(0.0, 1.0, 0.0) };
		testCube3 = new Object(test3, "angry.bmp");
		transformation_Matrices test4 = { glm::vec3(2.0, 3.0, -2.0), glm::vec3(0.8, 0.8, 0.8), nullTest };
		testCube4 = new Object(test4);
	}

	void init(void) {
		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		modelProgram = ShaderManager::initShaders("modelLoading.vert", "modelLoading.frag");
		ourModel = new Model("Nanosuit/nanosuit.obj");
	//	ourModel2 = new Model("gun/wep.obj");
		ourModel2 = new Model("CHOO/Socom pistol.obj");
		//shader = new Shader("modelLoading.vert", "modelLoading.frag");
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, greenMaterial);
		
		initObjects();
		h_manager = new hudManager();
		skybox = new Skybox(testTexFiles);
		physicsManager = new Physics();


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void renderObject(glm::mat4 proj, Model *modelData) {
		glUseProgram(modelProgram);
		mvStack.push(mvStack.top());// push modelview to stack
//		glCullFace(GL_BACK);
		MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));
	//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model
		glm::mat4 model;
	//	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);
		
		mvStack.pop();
	//	glCullFace(GL_BACK);
	}

	void renderWep(glm::mat4 proj, Model *modelData) {
		glUseProgram(modelProgram);
		mvStack.push(mvStack.top());// push modelview to stack
									//		glCullFace(GL_BACK);
		MeshManager::setLight(modelProgram, testLight);
		MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));
		//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);

		mvStack.pop();
		//	glCullFace(GL_BACK);
	}

	void update(SDL_Window * window) {
		controls(window);
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
		glDepthMask(GL_TRUE);
		camera();
		projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), SCREENWIDTH / SCREENHEIGHT, 1.0f, 100.0f);

		mvStack = skybox->renderSkybox(projection, mvStack, testCube->object_getMesh(), testCube->object_getIndex());

		mvStack = testCube->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0);

		mvStack = testCube2->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0);
	
		if (testMove.x >= 1.0) movement = -0.05;
		else if (testMove.x <= -1.0) movement = 0.05;
		testMove.x += movement;
		testCube2->setPosition(testMove);

		theta += 0.1f;
		mvStack = testCube3->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, theta);

		glm::vec3 currentPos = testCube4->getPosition();
		currentPos.y = physicsManager->applyGravity(currentPos);
		testCube4->setPosition(currentPos);
		mvStack = testCube4->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, theta);

//		renderTest(projection);
		renderWep(projection, ourModel2);
		renderObject(projection,ourModel);
		//:thinking:

		mvStack.pop();
		// h_manager->renderFPS(texturedProgram, testLight, glm::mat4(1.0), testCube->object_getMesh(), testCube->object_getIndex(), fps);

		glDepthMask(GL_TRUE);
		SDL_GL_SwapWindow(window); // swap buffers
	}
}