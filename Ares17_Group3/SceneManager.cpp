#include "SceneManager.h"
#include "Model.h"
//#include "Shader.h"
using namespace std;

#define objectNumber 7
#define BULLET_NO 30

typedef std::pair < glm::vec3, glm::vec3 > vectorPair;
typedef stack<glm::mat4> mvstack;

// this class still needs a lot of work
namespace SceneManager {
	/*
	Object *testCube;
	Object *testCube2;
	Object *testCube3;
	Object *testCube4;*/ // cubes were starting to get out of hand
	Object *testCubes[objectNumber]; // arrays :D
	Object *bullet[BULLET_NO];
	bool shotsFired = false;
	int noShotsFired = 0;
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

	transformation_Matrices testTransformation = { transTest, scaleTest, nullTest };

	float angleTest;

	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	// Setup and compile our shaders
//	Shader *shader;

	// Load models
	Model *ourModel;
	Model *ourModel2;

	mvstack mvStack;

	GLfloat camRotation = 0.0f;
	GLfloat camy = 0.0f;

	glm::vec3 eye(0.0f, 1.0f, 0.0f);
	glm::vec3 at(0.0f, 0.0f, -1.0f);
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

	bool leftClick = false;
	bool rightClick = false;
	
	void bulletCreation() {
		//position, scale, rotation
		glm::vec3 bulletSpawn = eye;
		bulletSpawn = moveForward(eye, camRotation, 0.5f);
		transformation_Matrices bulletTest = { bulletSpawn, glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.0, 0.0, 0.0) };
		bullet[noShotsFired] = new Object(bulletTest);
		bullet[noShotsFired]->setAngle(camRotation);
		bullet[noShotsFired]->setVelocity(glm::vec3(0.1, 0.1, 0.1));

		////EXPERIMENTING
		bullet[noShotsFired]->setAt(at);
		bullet[noShotsFired]->setInitialPosition(bulletSpawn);
		noShotsFired++;
	}

	bool collision(glm::vec3 position_A, glm::vec3 scale_A, glm::vec3 position_B, glm::vec3 scale_B) {

		bool collision = false;

		if ((position_A.x + (scale_A.x )) >= (position_B.x - (scale_B.x )) // if right side on the right of left side
			&& position_A.x - (scale_A.x ) <= (position_B.x + (scale_B.x ))) // and left side is left of right side
		{
			// if possible collision in x, then check z
			if ((position_A.z + (scale_A.z)) >= (position_B.z - (scale_B.z)) // if right side on the right of left side
				&& position_A.z - (scale_A.z) <= (position_B.z + (scale_B.z))) // and left side is left of right side
			{
				if ((position_A.y + (scale_A.y)) >= (position_B.y - (scale_B.y)) // if right side on the right of left side
					&& position_A.y - (scale_A.y) <= (position_B.y + (scale_B.y))) // and left side is left of right side
					collision = true;
			}	
		}

		return collision;
		
	}

	void bulletFunction(Object *bullet) {
	//	float increment = 0.1;
		glm::vec3 bulletPos = bullet->getPosition();
		glm::vec3 bulletScale = bullet->getScale();
		//glm::vec3 ObjectsPos = testCubes[0]->getPosition();
		//glm::vec3 ObjectsScale = testCubes[0]->getScale();

		glm::vec3 ObjectsPos[objectNumber];
		glm::vec3 ObjectsScale[objectNumber];
		for (int i = 0; i < objectNumber; i++){
			ObjectsPos[i] = testCubes[i]->getPosition();
			ObjectsScale[i] = testCubes[i]->getScale();
		}
		for (int i = 1; i < objectNumber; i++) {
			/*if (bulletPos.x > ObjectsPos[i].x + ObjectsScale[i].x ||
				bulletPos.z > ObjectsPos[i].z + ObjectsScale[i].z ||
				bulletPos.x < ObjectsPos[i].x - ObjectsScale[i].x ||
				bulletPos.z < ObjectsPos[i].z - ObjectsScale[i].z) {
				cout<<"collision"<<endl;//delete bullet;
			}*/
			/*			if (bulletPos.x >= ObjectsPos[i].x - ObjectsScale[i].x && bulletPos.x <= ObjectsPos[i].x + ObjectsScale[i].x)
							if(bulletPos.z >= ObjectsPos[i].z - ObjectsScale[i].z && bulletPos.z <= ObjectsPos[i].z + ObjectsScale[i].z)
								cout << "collision" << endl;//delete bullet;

				*/		
			if (collision(ObjectsPos[i], ObjectsScale[i], bulletPos, bulletScale)) {
			//	cout << "colliding" << endl;
				bullet->setVelocity(glm::vec3(0.0, 0.0, 0.0));
			//	bullet->~Object();
			}
			else {
				float angleAtShot = bullet->getAngle();
				glm::vec3 newPos = glm::vec3(bulletPos.x + bullet->getVelocity().x*std::sin(angleAtShot*DEG_TO_RADIAN),
					////EXPERIMENTING
					//bulletPos.y + bullet->getVelocity().y*std::atan2(bullet->getAt().y - bulletPos.y , bullet->getAt().x - bulletPos.x)  /*need the right math for this*/,
					//bulletPos.y + bullet->getVelocity().y*sin(std::atan2(bullet->getAt().y-bullet->getInitialPosition().y, bullet->getAt().x-bullet->getInitialPosition().x)),
					bulletPos.y,
					bulletPos.z - bullet->getVelocity().z*std::cos(angleAtShot*DEG_TO_RADIAN));
				bullet->setPosition(newPos);
			}
		}
		/*if (bulletPos.x > ObjectsPos.x + ObjectsScale.x ||
			bulletPos.z > ObjectsPos.z + ObjectsScale.z ||
			bulletPos.x < ObjectsPos.x - ObjectsScale.x ||
			bulletPos.z < ObjectsPos.z - ObjectsScale.z) {
			;//delete bullet;
		}*/
		
	}

	void controls(SDL_Window * window, SDL_Event sdlEvent) {
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

		///////////////////////////////////
		////////////MOUSE CLICK////////////
		///////////////////////////////////
		if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) leftClick = true;
			if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
		}
		
		if (leftClick == true) {
			if (noShotsFired >= BULLET_NO)
				cout << "no more ammo";
			else {
				shotsFired = true;
				bulletCreation();
			}
		}
		
		if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
			leftClick = false;
			rightClick = false;
		}
		
		///////////////////////////////////
		////////////KEYBOARD///////////////
		///////////////////////////////////
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, camRotation, 0.1f);
		else if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, camRotation, -0.1f);
		if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, camRotation, -0.1f);
		else if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, camRotation, 0.1f);
		if (keys[SDL_SCANCODE_R]) eye.y += 0.1;
		else if (keys[SDL_SCANCODE_F]) eye.y -= 0.1;

	//	if (keys[SDL_SCANCODE_COMMA]) camRotation -= 1.0f;
	//	else if (keys[SDL_SCANCODE_PERIOD]) camRotation += 1.0f;

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
		testCubes[0] = new Object(testTransformation);
		transformation_Matrices test2 = { testMove, glm::vec3(0.5, 0.5, 0.5), nullTest };
		//	transformation_Matrices(testMove, glm::vec3(0.5, 0.5, 0.5), nullTest)
		testCubes[1] = new Object(test2, "studdedmetal.bmp");
		transformation_Matrices test3 = { glm::vec3(-3.0, 2.0, 0.0), glm::vec3(0.5, 1.5, 0.5), glm::vec3(0.0, 1.0, 0.0) };
		testCubes[2] = new Object(test3);
		transformation_Matrices test4 = { glm::vec3(2.0, 10.0, -2.0), glm::vec3(0.8, 0.8, 0.8), nullTest };
		testCubes[3] = new Object(test4);
		transformation_Matrices test5 = { glm::vec3(2.0,4.0, 2.0), glm::vec3(1.5,1.5,1.5), nullTest };
		testCubes[4] = new Object(test5);
		transformation_Matrices testScale = { glm::vec3(1.0,4.0,5.0),nullTest,nullTest };
		transformation_Matrices testScale2 = { glm::vec3(0.5,4.5,3.0),glm::vec3(0.5,0.5,0.5),nullTest };
		testCubes[5] = new Object(testScale);
		testCubes[6] = new Object(testScale2);
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
	}

	/*void renderWep(glm::mat4 proj, Model *modelData) {
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
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);

		mvStack.pop();
		//	glCullFace(GL_BACK);
	}*/
	void renderWep(glm::mat4 proj, Model *modelData) {
		glUseProgram(modelProgram);
		glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
		mvStack.push(glm::mat4(1.0));// push modelview to stack
									//		glCullFace(GL_BACK);
		MeshManager::setLight(modelProgram, testLight);
		MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));
		//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model
		glBindTexture(GL_TEXTURE_2D, testCubes[0]->object_getTexture());
		glm::mat4 model;
		if (rightClick) {
			glm::vec3 gun_pos(0.0f, -2.0f, -5.0f);
			float Y_axisRotation = -85.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); // Translate it down a bit so it's at the center of the scene
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
		//	model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		}
		else {
			glm::vec3 gun_pos(2.5f, -2.5f, -5.0f);
			float Y_axisRotation = -50.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); // Translate it down a bit so it's at the center of the scene
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
			model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		}
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);

		mvStack.pop();
		//	glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 
		glDepthMask(GL_TRUE);
	}

	void moveObjects() {
		// MOVEMENT TESTING (testcube 2)
		if (testMove.x >= 1.0) movement = -0.05;
		else if (testMove.x <= -1.0) movement = 0.05;
		testMove.x += movement;
		testCubes[1]->setPosition(testMove);

		// rotate rotating cube (testcube 3)
		theta += 0.1f;

		// move testcube 4
		vectorPair currentProperties = make_pair(testCubes[3]->getPosition(), testCubes[3]->getVelocity());
		glm::vec3 gravity = glm::vec3(0.0, -2.0, 0.0);
		currentProperties = physicsManager->applyGravity(currentProperties.first, currentProperties.second, gravity);
		testCubes[3]->setPosition(currentProperties.first);
		testCubes[3]->setVelocity(currentProperties.second);

	}
	/*
	void moveBullets(int i) {
		vectorPair currentProperties = make_pair(bullet[i]->getPosition(), bullet[i]->getVelocity());
	//	glm::vec3 gravity = glm::vec3(0.0, -2.0, 0.0);
	//	currentProperties = physicsManager->applyGravity(currentProperties.first, currentProperties.second, gravity);
		bullet[i]->setPosition(currentProperties.first);
		bullet[i]->setVelocity(currentProperties.second);
	}
	*/
	void update(SDL_Window * window, SDL_Event sdlEvent) {
		controls(window, sdlEvent);
		moveObjects();
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
		projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), SCREENWIDTH / SCREENHEIGHT, 0.1f, 100.0f);

		mvStack = skybox->renderSkybox(projection, mvStack, testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex());

		mvStack = testCubes[0]->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0);

		mvStack = testCubes[1]->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0);
	
		mvStack = testCubes[2]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, theta);
	
		mvStack = testCubes[3]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0);

		mvStack = testCubes[5]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0);
		mvStack = testCubes[6]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0);

		if (shotsFired) {
			for (int i = 0; i < noShotsFired; i++) {
			//	moveBullets(i);
				mvStack = bullet[i]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0);
				bulletFunction(bullet[i]);
			}
		}
		// RENDERING MODELS
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