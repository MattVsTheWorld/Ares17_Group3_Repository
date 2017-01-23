#include "SceneManager.h"
#include "Model.h"
using namespace std;

//typedef std::pair < glm::vec3, glm::vec3 > vectorPair;
typedef stack<glm::mat4> mvstack;

// this class still needs a lot of work
namespace SceneManager {
	Object *testCube; 
	vector<Bullet*> bullet;
	Player *player;
	glm::vec3 playerScale(1.0, 3.0, 1.0);
	
	// Shaders
	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;
	
	hudManager *h_manager;
	Skybox *skybox;

	float SCREENWIDTH = 800.0f;
	float SCREENHEIGHT = 600.0f;
	unsigned int lastTime = 0, currentTime;
	enum pov { FIRST_PERSON, THIRD_PERSON };
	pov pointOfView = FIRST_PERSON;

	glm::vec3 transTest = glm::vec3(-10.0f, -1.5f, -10.0f);		
	glm::vec3 scaleTest = glm::vec3(20.0f, 0.5f, 20.0f);
	glm::vec3 nullTest = glm::vec3(0.0f, 0.0f, 0.0f);

	transformation_Matrices testTransformation = { transTest, scaleTest, nullTest, nullTest, nullTest };
	object_Properties cube;

	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	// Load models
	Model *ourModel;
	Model *ourModel2;

	mvstack mvStack;

	GLfloat pitch = 0.0f;
	GLfloat yaw = 0.0f;
	GLfloat roll = 0.0f;
	
	glm::vec3 eye(2.0f, 3.0f, -6.0f);
	glm::vec3 at(0.0f, 0.5f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	MeshManager::lightStruct testLight = {
		{ 0.6f, 0.4f, 0.6f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ 0.0f, 5.0f, 0.0f, 1.0f }  // position
	};
	glm::vec4 lightPos(0.0, 5.0, 0.0, 1.0);

	MeshManager::materialStruct greenMaterial = {
		{ 0.6f, 0.4f, 0.2f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};

	MeshManager::materialStruct redMaterial = {
		{ 0.6f, 0.4f, 0.4f, 1.0f }, // ambient
		{ 0.6f, 0.4f, 0.4f, 1.0f }, // diffuse
		{ 0.6f, 0.4f, 0.4f, 1.0f }, // specular
		2.0f  // shininess
	};

	MeshManager::materialStruct defaultMaterial = {
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // ambient
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // diffuse
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // specular
		2.0f  // shininess
	};
	


	// 18/01
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase; //improves collision check (?) // can improve by know world size (?)
	btConstraintSolver* solver;
	std::vector<btRigidBody*> bodies;

	btRigidBody* addBox(float width, float height, float depth,float x, float y, float z, float mass)
	{
		btTransform t; 
		t.setIdentity();
		t.setOrigin(btVector3(x,y,z));
		btBoxShape* box = new btBoxShape(btVector3(width/2,height/2,depth/2));
		btVector3 inertia(0, 0, 0);
		if (mass != 0.0)
			box->calculateLocalInertia(mass, inertia);
		btMotionState* motion = new btDefaultMotionState(t);
	
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia); 
																		 
		btRigidBody* body = new btRigidBody(info);
		world->addRigidBody(body);
		bodies.push_back(body);

		return body;
	}

	void renderBox(btRigidBody* box, glm::mat4 projection) {

		if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE)
		{
			cout << "Wrong collision shape (?)";
			return;
		}
		glUseProgram(shaderProgram);
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, defaultMaterial);
		mvStack.push(mvStack.top());// push modelview to stack
		
		btVector3 extent = ((btBoxShape*)box->getCollisionShape())->getHalfExtentsWithMargin();
		btTransform t;
		box->getMotionState()->getWorldTransform(t);

		//https://www.youtube.com/watch?v=1CEI2pOym1Y || 48 min ~~~

		glm::mat4 mat;
		t.getOpenGLMatrix(glm::value_ptr(mat));

		// https://www.youtube.com/watch?v=1CEI2pOym1Y  41~
		mvStack.top() *= mat; // trans, rot?
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(extent.x(), extent.y(), extent.z())); //DEFINITELY goes after

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testCube->object_getTexture());
		MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
		
		MeshManager::drawIndexedMesh(testCube->object_getMesh(), testCube->object_getIndex(), GL_TRIANGLES);
		glBindTexture(GL_TEXTURE_2D, 0);
		mvStack.pop();
	}

	void renderPlane(btRigidBody* plane, glm::mat4 projection)
	{
		if (plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
		{
			return;
			cout << "Wrong collision shape (?)";
		}
		//glColor3f(0.8, 0.8, 0.8);
		glUseProgram(shaderProgram);
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, defaultMaterial);
		mvStack.push(mvStack.top());// push modelview to stack

		btTransform t;
		plane->getMotionState()->getWorldTransform(t);
		
		t.getOpenGLMatrix(glm::value_ptr(mvStack.top()));

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testCubes[0]->object_getTexture());
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(10, 0.5, 10));
		MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
		//cout << "GASGHDJ";

		MeshManager::drawIndexedMesh(testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), GL_TRIANGLES);
		glBindTexture(GL_TEXTURE_2D, 0);
		*/
		mvStack.pop();

	}
	// +++++

	void initBoxes() {
		addBox(50, 0.1, 50, 0, 0, 0, 0.0);
		addBox(1.0f, 1.0f, 1.0f, 0, 10, -10, 1.0);
		bodies[1]->setActivationState(DISABLE_DEACTIVATION); // disable deactivation of physics on an object
															 // objects become static after ~2(?) seconds otherwise
		addBox(3.0f, 1.0f, 3.0f, 0, 0, -5, 1.0);
		bodies[2]->setActivationState(DISABLE_DEACTIVATION);

		addBox(playerScale.x, playerScale.y, playerScale.z, eye.x, eye.y, eye.z, 0);
		bodies[3]->setActivationState(DISABLE_DEACTIVATION);
	}

	void initPlayer() {
		transformation_Matrices playerTrans = { eye, playerScale, nullTest , nullTest , nullTest };
		player = new Player(playerTrans);
		
	}

	void init(void) {
		// 18/01
		collisionConfig = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfig); //base algorithm good (?)
		broadphase = new btDbvtBroadphase(); //divide space into different spaces // can use a more performancy ones // axis sweep?
		solver = new btSequentialImpulseConstraintSolver(); // can use OpenCL, multithreading (?)

		world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
		world->setGravity(btVector3(0, -10, 0)); // x y z as usual

		// ground as a plane
		// only used for debug (?)
		/*
		btTransform t; // orientation and position // quaternion :))))) """SIMPLE VARIABLE"""
		t.setIdentity(); // 0 position, 0 rotation
		t.setOrigin(btVector3(0, 0, 0));
		btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0,1,0),0); // 1st == normal (0,1,0) -> pointing upwards // second == distance from origin = 0 (AA) //infinite in every direction
		//motion state, although position for this is already set
		btMotionState* motion = new btDefaultMotionState(t);
		// mass = 0 == static, mass > 0 == dinamic ( < 0 no sense)
		btRigidBody::btRigidBodyConstructionInfo info(0,motion,plane); // no need to dynamically allocate
		// for dinamic bodies will have to add inertia (?)
		btRigidBody* body = new btRigidBody(info);
		world->addRigidBody(body);
		bodies.push_back(body);
		*/
	
		initBoxes();
		// +++++
		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		modelProgram = ShaderManager::initShaders("modelLoading.vert", "modelLoading.frag");
		ourModel = new Model("Nanosuit/nanosuit.obj");
		ourModel2 = new Model("CHOO/Socom pistol.obj");
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, greenMaterial);
		cube = testCube->initializeObject("cube.obj");

		//initObjects();
		testCube = new Object(testTransformation, cube);
		
		initPlayer();
		h_manager = new hudManager();
		skybox = new Skybox(testTexFiles);


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(yaw*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(yaw*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(yaw*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(yaw*DEG_TO_RADIAN));
	}

	float gameTime() {
		currentTime = clock();

		unsigned int dt = currentTime - lastTime;
		float dt_secs = (float)dt / 1000;
		if (dt_secs > 0.017f) dt_secs = 0.017f; // first value is off ( 5.5~)
											  
		lastTime = currentTime;

		return dt_secs;
	}

	void lockCamera()
	{
		if (pitch > 70)
			pitch = 70;
		if (pitch < -70)
			pitch = -70;
		if (yaw < 0.0)
			yaw += 360.0;
		if (yaw > 360.0)
			yaw -= 360;
	}

	bool leftClick = false;
	bool rightClick = false;

	void controls(SDL_Window * window, SDL_Event sdlEvent) {
		int MidX = SCREENWIDTH / 2;
		int MidY = SCREENHEIGHT / 2;

		SDL_ShowCursor(SDL_DISABLE);
		int tmpx, tmpy;
		SDL_GetMouseState(&tmpx, &tmpy);
		yaw -= 0.1*(MidX - tmpx); //for y
		pitch -= 0.1*(MidY - tmpy) / 10; //for x
		lockCamera();

		//rotate the camera (move everything in the opposit direction)
		glRotatef(-pitch, 1.0, 0.0, 0.0); //basically glm::rotate
		glRotatef(-yaw, 0.0, 1.0, 0.0);
		SDL_WarpMouseInWindow(window, MidX, MidY);
		
		//MOUSECLICK
		
		if (sdlEvent.type == SDL_MOUSEBUTTONDOWN && pointOfView == FIRST_PERSON) {
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) leftClick = true;
			if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
		}

		if (leftClick == true && pointOfView == FIRST_PERSON) {
			// Do something
		}

		if (sdlEvent.type == SDL_MOUSEBUTTONUP  && pointOfView == FIRST_PERSON) {
			leftClick = false;
			rightClick = false;
		}
		
		//KEYBOARD
		// To be changed ****
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) {
			player->setPosition(moveForward(player->getPosition(), yaw, 0.1f));
		}
		else if (keys[SDL_SCANCODE_S]) {
			player->setPosition(moveForward(player->getPosition(), yaw, -0.1f));
		} //else player->setVelocity(glm::vec3(0.0, player->getVelocity().y, player->getVelocity().z));
		if (keys[SDL_SCANCODE_A]) {
			player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
		}
		else if (keys[SDL_SCANCODE_D]) {
			player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
		}
		if (keys[SDL_SCANCODE_R]) {
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y + 0.1, player->getPosition().z));
		}
		else if (keys[SDL_SCANCODE_F]) {
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y - 0.1, player->getPosition().z));
		}

		if (keys[SDL_SCANCODE_SPACE] && player->getState() != JUMPING) {
			player->setVelocity(glm::vec3(player->getVelocity().x, 6.0f, player->getVelocity().z));
			player->setState(JUMPING);
		}
		//++++
		if (keys[SDL_SCANCODE_M]) {
			cout << "Curiously cinnamon\n";
			bodies[1]->setLinearVelocity(btVector3(0.0, 5.0, 0.0));
		}
		if (keys[SDL_SCANCODE_N]) {
			
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3(0, 10, -10));
			btMotionState* motion = new btDefaultMotionState(t);
			cout << "Setting y\n";
			bodies[1]->setMotionState(motion);
		}
		//++++
		if (keys[SDL_SCANCODE_K]) {
			if (pointOfView == FIRST_PERSON) pointOfView = THIRD_PERSON;
			else pointOfView = FIRST_PERSON;
		}

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

	//	if (keys[SDL_SCANCODE_5])cout << bullet.size() << endl;

		if (keys[SDL_SCANCODE_M]) {
			//cout << "Curiously cinnamon\n";
			bodies[1]->setLinearVelocity(btVector3(0.0, 5.0, 0.0));
		}
		if (keys[SDL_SCANCODE_KP_8])
			bodies[1]->setLinearVelocity(btVector3(5.0, 0.0, 0.0));
		if (keys[SDL_SCANCODE_KP_4])
			bodies[1]->setLinearVelocity(btVector3(0.0, 0.0, 5.0));
		if (keys[SDL_SCANCODE_KP_6])
			bodies[1]->setLinearVelocity(btVector3(0.0, 0.0, -5.0));
		if (keys[SDL_SCANCODE_KP_5])
			bodies[1]->setLinearVelocity(btVector3(-5.0, 0.0, 0.0));
	}

	void renderObject(glm::mat4 proj, Model *modelData, glm::vec3 pos) {
		glUseProgram(modelProgram);
		mvStack.push(mvStack.top());// push modelview to stack
		MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(mvStack.top()));
	//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model

		glm::mat4 model;
		model = glm::translate(model, pos);
		model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(180*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);
		
		mvStack.pop();
	}

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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testCube->object_getTexture());
		glm::mat4 model;
		/*
		if (rightClick) {
			glm::vec3 gun_pos(0.0f, -2.0f, -5.0f);
			float Y_axisRotation = -85.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); 
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
		//	model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		} */
	 //	else {
			glm::vec3 gun_pos(2.5f, -2.5f, -5.0f);
			float Y_axisRotation = -50.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); 
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
			model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
	//	}
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(modelProgram);

		mvStack.pop();
		//	glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 
		glDepthMask(GL_TRUE);
	}

	void update(SDL_Window * window, SDL_Event sdlEvent) {
		controls(window, sdlEvent);

		// 18/01
		world->stepSimulation(1/60.0); // 1 divided by frames per second
		// would need to delete dispatcher, collisionconfig, solver, world, broadphase in main
		// +++++
	}

	void camera() {
		if (pointOfView == FIRST_PERSON) {
			at = moveForward(player->getPosition(), yaw, 1.0f);
			at.y -= pitch;
			mvStack.top() = glm::lookAt(player->getPosition(), at, up);
		}

		else {
			at = player->getPosition(); // look at player position
			eye = moveForward(at, pitch, -6.0f); // move behind him
			eye.y += pitch; // displacement determined by user interaction
			mvStack.top() = glm::lookAt(eye, at, up);
		}

		glm::vec4 tmp = mvStack.top()*lightPos;
		testLight.position[0] = tmp.x;
		testLight.position[1] = tmp.y;
		testLight.position[2] = tmp.z;
		MeshManager::setLightPos(shaderProgram, glm::value_ptr(tmp));
	}

	void draw(SDL_Window * window) {
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

		mvStack = skybox->renderSkybox(projection, mvStack, testCube->object_getMesh(), testCube->object_getIndex());
	
		///+++++++++++++++
		//renderPlane(bodies[0], projection);
		renderBox(bodies[0], projection);
		renderBox(bodies[1], projection);
		renderBox(bodies[2], projection);
		renderBox(bodies[3], projection);
		///+++++++++++++++

		// RENDERING MODELS
		if (pointOfView == THIRD_PERSON)
			renderObject(projection,ourModel, glm::vec3(player->getPosition().x, player->getPosition().y-1.5, player->getPosition().z));
		if (pointOfView == FIRST_PERSON)
			renderWep(projection, ourModel2);
		//:thinking:
		///
		//h_manager->renderToHud(11, texturedProgram, testLight, testCube->object_getMesh(), testCubes[0]->object_getIndex(), glm::vec3(-0.25f, 0.9f, 0.9f), testCubes[11]->getVelocity().x, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z);
		///
		mvStack.pop();

		glDepthMask(GL_TRUE);
		SDL_GL_SwapWindow(window); // swap buffers
	}
}
