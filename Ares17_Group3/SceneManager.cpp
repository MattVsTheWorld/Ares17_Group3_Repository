#include "SceneManager.h"
#include "Model.h"


//#include "Shader.h"
using namespace std;

#define OBJECT_NO 14
#define BULLET_NO 30
#define TEST_VELOCITY 0.5

typedef std::pair < glm::vec3, glm::vec3 > vectorPair;
typedef stack<glm::mat4> mvstack;

// this class still needs a lot of work
namespace SceneManager {



	Object *testCubes[OBJECT_NO]; // arrays :D
	Object *boundingBoxes[OBJECT_NO];

	vector<Bullet*> bullet;

	Player *player;
	glm::vec3 playerScale(0.5, 1.5, 0.5);
	Object *playerBox;
	bool shotsFired = false;
	int noShotsFired = 0;
	float theta = 0.0f;
	float adjustable_Angle = 0.0f;
	bool viewBoxes = false;
	float movement = 0.05f;
	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;
	hudManager *h_manager;
	Skybox *skybox;
	Physics *physicsManager;
	Collisions *collisionManager;

	float SCREENWIDTH = 800.0f;
	float SCREENHEIGHT = 600.0f;
	unsigned int lastTime = 0, currentTime;
	enum pov { FIRST_PERSON, THIRD_PERSON };
	pov pointOfView = FIRST_PERSON;

	glm::vec3 transTest = glm::vec3(-10.0f, -0.1f, -10.0f);		
	glm::vec3 scaleTest = glm::vec3(20.0f, 0.1f, 20.0f);
	glm::vec3 nullTest = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pitchTest = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 yawTest = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rollTest = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 testMove = glm::vec3(0.0, 2.0, -2.0);
	glm::vec3 collisionPosition;

	transformation_Matrices testTransformation = { transTest, scaleTest, nullTest, nullTest, nullTest };
	object_Properties cube;

	float angleTest;

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

	void initObjects() {
		testCubes[0] = new Object(testTransformation, cube);
		transformation_Matrices test2 = { testMove, glm::vec3(0.5, 0.5, 0.5), nullTest };
		//	transformation_Matrices(testMove, glm::vec3(0.5, 0.5, 0.5), nullTest)
		transformation_Matrices test0 = { glm::vec3(0.0, 2.0, -4.0), glm::vec3(0.5, 0.5, 0.5), nullTest };
		testCubes[1] = new Object(test0, "studdedmetal.bmp", cube);
		testCubes[1]->setVelocity(glm::vec3(TEST_VELOCITY, 0.0, 0.0));
		transformation_Matrices test7 = { glm::vec3(-2.0, 2.0, -4.0), glm::vec3(0.5, 0.5, 0.5), nullTest };
		testCubes[7] = new Object(test7, "studdedmetal.bmp", cube);
		testCubes[7]->setVelocity(glm::vec3(TEST_VELOCITY, 0.0, 0.0));
		transformation_Matrices test8 = { glm::vec3(-1.0, 2.0, -4.0), glm::vec3(0.25, 0.25, 0.25), nullTest };
		testCubes[8] = new Object(test8, cube);
		transformation_Matrices test3 = { glm::vec3(-3.0, 2.0, 0.0), glm::vec3(0.5, 1.5, 0.5), pitchTest, yawTest };
		testCubes[2] = new Object(test3, cube);
		transformation_Matrices test4 = { glm::vec3(2.0, 10.0, -2.0), glm::vec3(0.8, 0.8, 0.8), nullTest };
		testCubes[3] = new Object(test4, cube);
		transformation_Matrices test5 = { glm::vec3(6.0,4.0, 2.0), glm::vec3(1.5,1.5,1.5), nullTest };
		testCubes[4] = new Object(test5, cube);
		transformation_Matrices testScale = { glm::vec3(1.0,4.0,5.0),glm::vec3(0.8, 0.8, 0.8),nullTest };
		transformation_Matrices testScale2 = { glm::vec3(0.5,4.5,3.0),glm::vec3(0.5,0.5,0.5),nullTest };
		testCubes[5] = new Object(testScale, cube);
		testCubes[6] = new Object(testScale2, cube);

		transformation_Matrices test9 = { glm::vec3(-4.0, 1.0, -8.0), glm::vec3(0.5, 0.5, 0.5), nullTest };
		testCubes[9] = new Object(test9, "move.bmp", cube);

		transformation_Matrices testCollidable = { glm::vec3(-5.0,5.0,-5.0), glm::vec3(0.8,0.5,0.8), pitchTest, yawTest };
		testCubes[10] = new Object(testCollidable, cube);

		transformation_Matrices collisionTests[3] = {
			{ glm::vec3(-7.0,1.0,-8.0), glm::vec3(0.5,0.5,0.5), pitchTest, yawTest },
			{ glm::vec3(-10.0,1.0,-7.0), glm::vec3(0.5,0.5,0.5), pitchTest, yawTest },
			{ glm::vec3(-9.0,1.0,-9.0), glm::vec3(0.5,0.5,0.5), pitchTest, yawTest }
		};

		testCubes[11] = new Object(collisionTests[0], "11.bmp", cube);
		testCubes[12] = new Object(collisionTests[1], "12.bmp", cube);
		testCubes[13] = new Object(collisionTests[2], "13.bmp", cube);


		for (int i = 0; i < OBJECT_NO; i++)
			boundingBoxes[i] = testCubes[i];

	}

	void initPlayer() {
		transformation_Matrices playerTrans = { eye, playerScale, nullTest , nullTest , nullTest };
		player = new Player(playerTrans);
		playerBox = new Object (playerTrans, cube);
	}

	// 18/01
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase; //improves collision check (?) // can improve by know world size (?)
	btConstraintSolver* solver;
	std::vector<btRigidBody*> bodies;

	btRigidBody* addCube(btVector3 &scale, btVector3 &pos, float mass)
	{
		btTransform t; // orientation and position // quaternion :))))) """SIMPLE VARIABLE"""
		t.setIdentity(); // 0 position, 0 rotation
		t.setOrigin(pos);
		btBoxShape* box = new btBoxShape(scale);
		btVector3 inertia(0, 0, 0);
		if (mass != 0.0)
			box->calculateLocalInertia(mass, inertia);
		btMotionState* motion = new btDefaultMotionState(t);
		// mass = 0 == static, mass > 0 == dinamic ( < 0 no sense)
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia); 
																		 
		btRigidBody* body = new btRigidBody(info);
		world->addRigidBody(body);
		bodies.push_back(body);

		return body;
	}

	void renderBox(btRigidBody* box, glm::vec3 pos, glm::mat4 projection) {

		if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE)
		{
			cout << "Wrong collision shape (?)";
			return;
		}
		glUseProgram(shaderProgram);
		MeshManager::setLight(shaderProgram, testLight);
		mvStack.push(mvStack.top());// push modelview to stack
		mvStack.top() = glm::translate(mvStack.top(), pos);
		//btVector3 scale = (btBoxShape*)box->getCollisionShape()->getMargin();
		//btTransform t;
		//box->getMotionState()->getWorldTransform(t);
		//float mat[16];
		//t.getOpenGLMatrix(mat);
		
		//bunch of outdated matrix stuff

		// https://www.youtube.com/watch?v=1CEI2pOym1Y || 48 min ~~~

		//mvStack.top() = glm::scale(mvStack.top(), );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testCubes[0]->object_getTexture());
		MeshManager::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		MeshManager::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
		MeshManager::setMaterial(shaderProgram, defaultMaterial);
		MeshManager::drawIndexedMesh(testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), GL_TRIANGLES);
		glBindTexture(GL_TEXTURE_2D, 0);
		mvStack.pop();
	}
	// +++++
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

		addCube(btVector3(1.0,1.0,1.0), btVector3(-10,3.0,-10), 1.0);


		// +++++
		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		modelProgram = ShaderManager::initShaders("modelLoading.vert", "modelLoading.frag");
		ourModel = new Model("Nanosuit/nanosuit.obj");
		ourModel2 = new Model("CHOO/Socom pistol.obj");
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, greenMaterial);
		cube = testCubes[0]->initializeObject("cube.obj");

		initObjects();

		initPlayer();
		h_manager = new hudManager();
		skybox = new Skybox(testTexFiles);
		physicsManager = new Physics();
		collisionManager = new Collisions();

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

	void bulletCreation() {
		//position, scale, rotation
		glm::vec3 bulletSpawn = player->getPosition();
		bulletSpawn = moveForward(player->getPosition(), yaw, 0.5f);
		glm::mat4 model;
		transformation_Matrices bulletTest = { bulletSpawn, glm::vec3(0.1, 0.1, 0.1),
			pitchTest, yawTest, rollTest };

		Bullet newBullet(bulletTest, cube);

		newBullet.setYawAngle(yaw);
		newBullet.setPitchAngle(pitch);
		newBullet.setVelocity(glm::vec3(0.1, 0.1, 0.1));
		noShotsFired++;
		bullet.push_back(new Bullet(newBullet)); //add at end		
	}

	/*
	bool collision(glm::vec3 position_A, glm::vec3 scale_A, glm::vec3 position_B, glm::vec3 scale_B) {

		bool collision = false;

		if ((position_A.x + (scale_A.x )) >= (position_B.x - (scale_B.x )) // if right side on the right of left side
			&& position_A.x - (scale_A.x ) <= (position_B.x + (scale_B.x ))) // and left side is left of right side
		{
			// if possible collision in x, then check z
			if ((position_A.z + (scale_A.z)) >= (position_B.z - (scale_B.z)) // if right side on the right of left side
				&& position_A.z - (scale_A.z) <= (position_B.z + (scale_B.z))) // and left side is left of right side
			{
				if ((position_A.y + (scale_A.y)) >= (position_B.y - (scale_B.y)) 
					&& position_A.y - (scale_A.y) <= (position_B.y + (scale_B.y))) 
					collision = true;
			}	
		}

		return collision;

	}
	*/
	void bulletFunction(int bulletAt) {

		glm::vec3 ObjectsPos[OBJECT_NO];
		glm::vec3 ObjectsScale[OBJECT_NO];
		for (int i = 0; i < OBJECT_NO; i++) {
			ObjectsPos[i] = testCubes[i]->getPosition();
			ObjectsScale[i] = testCubes[i]->getScale();
		}
		bool stop = false;

		for (int i = 1; i < OBJECT_NO; i++) {
			if (!stop) {
				if (collisionManager->doCollisions(ObjectsPos[i], ObjectsScale[i], bullet[bulletAt]->getPosition(), bullet[bulletAt]->getScale())) {
					bullet[bulletAt]->setVelocity(glm::vec3(0.0, 0.0, 0.0));
					vector<Bullet*>::iterator iter = bullet.begin() + bulletAt;
					delete *iter;
					bullet.erase(iter);
					cout << "Cube " << i << " hit. Bullet deleted.\n";
					stop = true;
				}
				else {
					float angleAtShot = bullet[bulletAt]->getYawAngle();
					glm::vec3 newPos = glm::vec3(bullet[bulletAt]->getPosition().x + bullet[bulletAt]->getVelocity().x*std::sin(angleAtShot*DEG_TO_RADIAN),
						bullet[bulletAt]->getPosition().y - bullet[bulletAt]->getVelocity().y*sin(std::atan(bullet[bulletAt]->getPitchAngle())),
						bullet[bulletAt]->getPosition().z - bullet[bulletAt]->getVelocity().z*std::cos(angleAtShot*DEG_TO_RADIAN));
					bullet[bulletAt]->setPosition(newPos);
				}
			}
		}
	}

	// will update for a more generalized and reliable method
	bool playerCollision(glm::vec3 eye, glm::vec3 playerScale) {
		bool p_coll = false;
		for (int i = 0; i < OBJECT_NO; i++) {
			p_coll = collisionManager->doCollisions(eye, playerScale, testCubes[i]->getPosition(), testCubes[i]->getScale());
			if (p_coll) {
				player->setVelocity(testCubes[i]->getVelocity());
				return p_coll;
			}
		}
		return p_coll;
	}

	// just an example of how shoving might work
	// actual method would probably account for velocities instead of positions
	void exampleShove() {
		bool test_coll = collisionManager->doCollisions(collisionPosition, player->getScale(), testCubes[9]->getPosition(), testCubes[9]->getScale());
		if (test_coll) {
			testCubes[9]->setPosition(glm::vec3(testCubes[9]->getPosition().x + (collisionPosition.x - player->getPosition().x),
				testCubes[9]->getPosition().y, // + (collisionPosition.y - player->getPosition().y),
				testCubes[9]->getPosition().z + (collisionPosition.z - player->getPosition().z)));
		}
	}


	float coolDownOfGun = 0.4f; //wait between shots

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
			if (coolDownOfGun <= 0.0f) {
				bulletCreation();
				coolDownOfGun = 0.4f;
				shotsFired = true;
			}
		}

		if (sdlEvent.type == SDL_MOUSEBUTTONUP  && pointOfView == FIRST_PERSON) {
			leftClick = false;
			rightClick = false;
		}
		//KEYBOARD
	
		
		
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) {
		//	player->setVelocity(glm::vec3(1.0, player->getVelocity().y, player->getVelocity().z));
			collisionPosition = moveForward(player->getPosition(), yaw, 0.1f);
			if(!playerCollision(collisionPosition, playerScale))
				player->setPosition(moveForward(player->getPosition(), yaw, 0.1f));
		}
		else if (keys[SDL_SCANCODE_S]) {
		//	player->setVelocity(glm::vec3(-1.0, player->getVelocity().y, player->getVelocity().z));
			collisionPosition = moveForward(player->getPosition(), yaw, -0.1f);
			if (!playerCollision(collisionPosition, playerScale))
				player->setPosition(moveForward(player->getPosition(), yaw, -0.1f));
		} //else player->setVelocity(glm::vec3(0.0, player->getVelocity().y, player->getVelocity().z));
		if (keys[SDL_SCANCODE_A]) {
			collisionPosition = moveRight(player->getPosition(), yaw, -0.1f);
			if (!playerCollision(collisionPosition, playerScale))
				player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
		}
		else if (keys[SDL_SCANCODE_D]) {
			collisionPosition = moveRight(player->getPosition(), yaw, 0.1f);
			if (!playerCollision(collisionPosition, playerScale))
				player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
		}
		if (keys[SDL_SCANCODE_R]) {
			collisionPosition = player->getPosition();
			collisionPosition.y += 0.1;
			if (!playerCollision(collisionPosition, playerScale))
				player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y + 0.1, player->getPosition().z));
		}
		else if (keys[SDL_SCANCODE_F]) {
			collisionPosition = player->getPosition();
			collisionPosition.y -= 0.1;
			if (!playerCollision(collisionPosition, playerScale))
				player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y - 0.1, player->getPosition().z));
		}

		if (keys[SDL_SCANCODE_SPACE] && player->getState() != JUMPING) {
			player->setVelocity(glm::vec3(player->getVelocity().x, 6.0f, player->getVelocity().z));
			player->setState(JUMPING);
		}
		if (keys[SDL_SCANCODE_K]) {
			if (pointOfView == FIRST_PERSON) pointOfView = THIRD_PERSON;
			else pointOfView = FIRST_PERSON;
		}

		if (keys[SDL_SCANCODE_O]) adjustable_Angle += 0.1;
		if (keys[SDL_SCANCODE_P]) adjustable_Angle -= 0.1;

		if (keys[SDL_SCANCODE_1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}
		if (keys[SDL_SCANCODE_2]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}

		if (keys[SDL_SCANCODE_3]) {
			viewBoxes = true;
		}
		if (keys[SDL_SCANCODE_4]) {
			viewBoxes = false;
		}
		if (keys[SDL_SCANCODE_ESCAPE]) {
			exit(0);
		}
		if (keys[SDL_SCANCODE_5])
		{
			cout << bullet.size() << endl;
		}

		if (keys[SDL_SCANCODE_KP_8])
			testCubes[11]->setVelocity(glm::vec3(testCubes[11]->getVelocity().x, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z - 0.2));
		if (keys[SDL_SCANCODE_KP_4])
			testCubes[11]->setVelocity(glm::vec3(testCubes[11]->getVelocity().x + 0.2, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z));
		if (keys[SDL_SCANCODE_KP_6])
			testCubes[11]->setVelocity(glm::vec3(testCubes[11]->getVelocity().x - 0.2, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z));
		if (keys[SDL_SCANCODE_KP_2])
			testCubes[11]->setVelocity(glm::vec3(testCubes[11]->getVelocity().x, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z + 0.2));

		if (keys[SDL_SCANCODE_KP_7])
			testCubes[12]->setVelocity(glm::vec3(testCubes[12]->getVelocity().x, testCubes[12]->getVelocity().y, testCubes[12]->getVelocity().z - 0.2));
		if (keys[SDL_SCANCODE_KP_9])
			testCubes[12]->setVelocity(glm::vec3(testCubes[12]->getVelocity().x + 0.2, testCubes[12]->getVelocity().y, testCubes[12]->getVelocity().z));
		if (keys[SDL_SCANCODE_KP_1])
			testCubes[12]->setVelocity(glm::vec3(testCubes[12]->getVelocity().x - 0.2, testCubes[12]->getVelocity().y, testCubes[12]->getVelocity().z));
		if (keys[SDL_SCANCODE_KP_3])
			testCubes[12]->setVelocity(glm::vec3(testCubes[12]->getVelocity().x, testCubes[12]->getVelocity().y, testCubes[12]->getVelocity().z + 0.2));
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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testCubes[0]->object_getTexture());
		glm::mat4 model;
		if (rightClick) {
			glm::vec3 gun_pos(0.0f, -2.0f, -5.0f);
			float Y_axisRotation = -85.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); 
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
		//	model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		}
		else {
			glm::vec3 gun_pos(2.5f, -2.5f, -5.0f);
			float Y_axisRotation = -50.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos); 
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

	

	bool once = true;

	void movePlayer(float dt_secs) {
		// gravity-y-y-y-y (https://www.youtube.com/watch?v=uQpclIzUwLk)
		glm::vec3 collisionPosition;
		glm::vec3 playerScale(0.5, 1.5, 0.5);
		vectorPair currentProperties = make_pair(player->getPosition(), player->getVelocity());
		glm::vec3 gravity = glm::vec3(0.0, -10.0, 0.0);
		if (player->getState() == JUMPING) 
			currentProperties = physicsManager->applyGravity(currentProperties.first, currentProperties.second, gravity, dt_secs);
		collisionPosition = currentProperties.first;
		if (!playerCollision(collisionPosition, playerScale))
		{
			player->setPosition(currentProperties.first);
			player->setState(JUMPING);
		}
		else {
			player->setState(ON_GROUND);
			player->setVelocity(glm::vec3(player->getVelocity().x, 0.0f, player->getVelocity().z));
		}
		
		if(player->getState() == JUMPING)
			player->setVelocity(glm::vec3(player->getVelocity().x, currentProperties.second.y, player->getVelocity().z));

		//PLAYER
		currentProperties = physicsManager->applyPhysics(player->getPosition(), player->getVelocity(), nullTest, dt_secs);
		player->setPosition(currentProperties.first);
		player->setVelocity(currentProperties.second);

	}

	bool objectCollision(Object*& object, glm::vec3 objectPos, glm::vec3 objectScale, int current) {
		bool o_coll = false;
		for (int i = 0; i < OBJECT_NO; i++) {
			if (i != current) {
				o_coll = collisionManager->doCollisions(objectPos, objectScale, testCubes[i]->getPosition(), testCubes[i]->getScale());
				if (o_coll) {
					if (testCubes[i]->getVelocity() != nullTest){ //&& testCubes[i]->getVelocity() != object->getVelocity()) {
						object->setVelocity(testCubes[i]->getVelocity());
					}
					return o_coll;
				} //else object->setVelocity(nullTest);
			}
		}
			return o_coll;
		
	}

	// over simplified; collision reaction is still not well implemented
	void shoveCubes() {
		objectCollision(testCubes[8], testCubes[8]->getPosition(), testCubes[8]->getScale(), 8);
	}

	///++++++++++++++++++

	enum Direction {
		PLUS_Z,
		PLUS_X,
		MINUS_Z,
		MINUS_X
	};

	Direction VectorDirection(glm::vec3 target) {
		glm::vec3 compass[] = {
			glm::vec3(0.0f, 0.0f, 1.0f),	// PLUS_Z
			glm::vec3(1.0f, 0.0f, 0.0f),	// PLUS_X
			glm::vec3(0.0f, 0.0f, -1.0f),	// MINUS_Z
			glm::vec3(-1.0f, 0.0f, 0.0f)	// MINUS_X
		};
		GLfloat max = 0.0f;
		GLuint best_match = -1;
		for (GLuint i = 0; i < 4; i++)
		{
			GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
			if (dot_product > max)
			{
				max = dot_product;
				best_match = i;
			}
		}
		return (Direction)best_match;
	}

	string textualize(Direction in) {
		if (in == 0)
			return "PLUS_Z";
		else if (in == 1)
			return "PLUS_X";
		else if (in == 2)
			return "MINUS_Z";
		else if (in == 3)
			return "MINUS_X";
		else return "~";
	}

	glm::vec3 nullVec(0.0, 0.0, 0.0);

	void collide_test(float dt_secs) {

		glm::vec3 friction_2 = glm::vec3(0.75, 0.75, 0.75);
		vectorPair currentProperties = make_pair(testCubes[11]->getPosition(), testCubes[11]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[11]->getPosition(), testCubes[11]->getVelocity(), friction_2, dt_secs);
		testCubes[11]->setPosition(currentProperties.first);
		testCubes[11]->setVelocity(currentProperties.second);

		currentProperties = make_pair(testCubes[12]->getPosition(), testCubes[12]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[12]->getPosition(), testCubes[12]->getVelocity(), friction_2, dt_secs);
		testCubes[12]->setPosition(currentProperties.first);
		testCubes[12]->setVelocity(currentProperties.second);

		currentProperties = make_pair(testCubes[12]->getPosition(), testCubes[13]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[13]->getPosition(), testCubes[13]->getVelocity(), friction_2, dt_secs);
		testCubes[13]->setPosition(currentProperties.first);
		testCubes[13]->setVelocity(currentProperties.second);

		bool coll = false;
		for (int current = 11; current < 14; current++)
		{
			for (int i = 11; i < 14; i++)
			{
				if (i != current) {
					coll = collisionManager->doCollisions((testCubes[current]->getPosition() + testCubes[current]->getVelocity()*dt_secs), testCubes[current]->getScale(), (testCubes[i]->getPosition() + testCubes[i]->getVelocity()*dt_secs), testCubes[i]->getScale());
					if (coll){						
					//	cout << textualize(VectorDirection((testCubes[current]->getVelocity()))) << " || ";
						if (VectorDirection((testCubes[current]->getVelocity())) == PLUS_Z || VectorDirection((testCubes[current]->getVelocity())) == MINUS_Z)
							testCubes[current]->setVelocity(glm::vec3(testCubes[current]->getVelocity().x, testCubes[current]->getVelocity().y, - (testCubes[current]->getVelocity().z)));
						if (VectorDirection((testCubes[current]->getVelocity())) == PLUS_X || VectorDirection((testCubes[current]->getVelocity())) == MINUS_X)
							testCubes[current]->setVelocity(glm::vec3(-(testCubes[current]->getVelocity().x), testCubes[current]->getVelocity().y, testCubes[current]->getVelocity().z));
		
						if (VectorDirection((testCubes[i]->getVelocity())) == PLUS_Z || VectorDirection((testCubes[i]->getVelocity())) == MINUS_Z)
							testCubes[i]->setVelocity(glm::vec3(testCubes[i]->getVelocity().x, testCubes[i]->getVelocity().y, - (testCubes[i]->getVelocity().z)));
						if (VectorDirection((testCubes[i]->getVelocity())) == PLUS_X || VectorDirection((testCubes[i]->getVelocity())) == MINUS_X)
							testCubes[i]->setVelocity(glm::vec3(-(testCubes[i]->getVelocity().x), testCubes[i]->getVelocity().y, testCubes[i]->getVelocity().z));
						//testCubes[i]->setVelocity(nullVec);
					}

				}
			}
		}
		

		//	testCubes[11];
		//	testCubes[12];
		//	testCubes[13];
	}
	
	///++++++++++++++++++

	void moveObjects() {
		float dt_secs = gameTime();
		// cout << dt_secs <<"\n";
		coolDownOfGun -= dt_secs;

		movePlayer(dt_secs);
		vectorPair currentProperties;
		
		//++
		collide_test(dt_secs);
		//++
		shoveCubes();
		exampleShove();
		// MOVEMENT TESTING (testcube 2)

		// green moving
		if (testCubes[1]->getPosition().x >= 1.0) testCubes[1]->setVelocity(glm::vec3(-TEST_VELOCITY, testCubes[1]->getVelocity().y, testCubes[1]->getVelocity().z));
		else if (testCubes[1]->getPosition().x <= -1.0) testCubes[1]->setVelocity(glm::vec3(TEST_VELOCITY, testCubes[1]->getVelocity().y, testCubes[1]->getVelocity().z));
		// probably make this a method since it's repeated so often
		currentProperties = make_pair(testCubes[1]->getPosition(), testCubes[1]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[1]->getPosition(), testCubes[1]->getVelocity(), nullTest, dt_secs);
		testCubes[1]->setPosition(currentProperties.first);
		testCubes[1]->setVelocity(currentProperties.second);

		// red moving
		if (testCubes[7]->getPosition().x >= -1.0) testCubes[7]->setVelocity(glm::vec3(-TEST_VELOCITY, testCubes[7]->getVelocity().y, testCubes[7]->getVelocity().z));
		else if (testCubes[7]->getPosition().x <= -3.0) testCubes[7]->setVelocity(glm::vec3(TEST_VELOCITY, testCubes[7]->getVelocity().y, testCubes[7]->getVelocity().z));

		currentProperties = make_pair(testCubes[7]->getPosition(), testCubes[7]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[7]->getPosition(), testCubes[7]->getVelocity(), nullTest, dt_secs);
		testCubes[7]->setPosition(currentProperties.first);
		testCubes[7]->setVelocity(currentProperties.second);

		//small cube
		currentProperties = make_pair(testCubes[8]->getPosition(), testCubes[8]->getVelocity());
		currentProperties = physicsManager->applyPhysics(testCubes[8]->getPosition(), testCubes[8]->getVelocity(), nullTest, dt_secs);
		testCubes[8]->setPosition(currentProperties.first);
		testCubes[8]->setVelocity(currentProperties.second);
		
		// rotate rotating cube (testcube 3 ([2]))
		theta += 0.1f;

		glm::vec3 speed = glm::vec3(-2.0, 0.0, -4.0);
		glm::vec3 friction = glm::vec3(speed.x / 7, 0.0, speed.z / 7);
		if (once) {
			testCubes[2]->setVelocity(speed);
			once = false;
		}
		
		if (testCubes[2]->getVelocity() != glm::vec3(0.0,0.0,0.0))
		{
			currentProperties = make_pair(testCubes[2]->getPosition(), testCubes[2]->getVelocity());
			currentProperties = physicsManager->applyPhysics(testCubes[2]->getPosition(), testCubes[2]->getVelocity(), friction, dt_secs);
			testCubes[2]->setPosition(currentProperties.first);
			testCubes[2]->setVelocity(currentProperties.second);
		}
		
		// move testcube 4
		currentProperties = make_pair(testCubes[3]->getPosition(), testCubes[3]->getVelocity());
		glm::vec3 gravity = glm::vec3(0.0, -2.0, 0.0);
		currentProperties = physicsManager->applyGravity(currentProperties.first, currentProperties.second, gravity, dt_secs);
		testCubes[3]->setPosition(currentProperties.first);
		testCubes[3]->setVelocity(currentProperties.second);

		for (int i = 0; i < OBJECT_NO; i++) {
			if (testCubes[i]->getVelocity() == glm::vec3(0.0, 0.0, 0.0))
				testCubes[i]->setState(STILL);
			else testCubes[i]->setState(MOVING);

		}

		// NEW +++
		for (int i = 0; i < OBJECT_NO; i++)
			boundingBoxes[i]->setPosition(testCubes[i]->getPosition());
		playerBox->setPosition(player->getPosition());



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

		// 18/01
		world->stepSimulation(1/60.0); // 1 divided by frames per second
		// would need to delete dispatcher, collisionconfig, solver, world, broadphase in main
		// +++++
		moveObjects();
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

		mvStack = skybox->renderSkybox(projection, mvStack, testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex());
																								
		//pitch, yaw, roll
		mvStack = testCubes[0]->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0, 0, 0);
		mvStack = testCubes[1]->renderObject(projection, mvStack, shaderProgram, testLight, greenMaterial, 0, 0, 0);
		mvStack = testCubes[2]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, theta, 0);
		mvStack = testCubes[3]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[4]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[5]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[6]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[7]->renderObject(projection, mvStack, shaderProgram, testLight, redMaterial, 0, 0, 0);
		mvStack = testCubes[8]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[9]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		
		// NEW +++++++++
		// Collision testing
		mvStack = testCubes[10]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, adjustable_Angle, 0);

		///
		// +++++++++++++++++++
		///
		mvStack = testCubes[11]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[12]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		mvStack = testCubes[13]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
		
		///
		// +++++++++++++++++++
		///
		if (viewBoxes) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			for (int i = 0; i < OBJECT_NO; i++)
				mvStack = boundingBoxes[i]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);
			mvStack = playerBox->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, 0, 0, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}
		// +++++++++++++++

		if (shotsFired) {
		for (int i = 0; i < bullet.size(); i++) {	
				mvStack = bullet[i]->renderObject(projection, mvStack, shaderProgram, testLight, defaultMaterial, bullet[i]->getPitchAngle(), bullet[i]->getYawAngle(), 0);
				bulletFunction(i);		
			}
		}
		// RENDERING MODELS
		if (pointOfView == THIRD_PERSON)
			renderObject(projection,ourModel, glm::vec3(player->getPosition().x, player->getPosition().y-1.5, player->getPosition().z));
		if (pointOfView == FIRST_PERSON)
			renderWep(projection, ourModel2);
		//:thinking:



		///
		h_manager->renderToHud(11, texturedProgram, testLight, testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), glm::vec3(-0.25f, 0.9f, 0.9f), testCubes[11]->getVelocity().x, testCubes[11]->getVelocity().y, testCubes[11]->getVelocity().z);
		h_manager->renderToHud(12, texturedProgram, testLight, testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), glm::vec3(-0.25f, 0.7f, 0.9f), testCubes[12]->getVelocity().x, testCubes[12]->getVelocity().y, testCubes[12]->getVelocity().z);
		h_manager->renderToHud(13, texturedProgram, testLight, testCubes[0]->object_getMesh(), testCubes[0]->object_getIndex(), glm::vec3(-0.25f, 0.5f, 0.9f), testCubes[13]->getVelocity().x, testCubes[13]->getVelocity().y, testCubes[13]->getVelocity().z);
		///


		mvStack.pop();


		glDepthMask(GL_TRUE);
		SDL_GL_SwapWindow(window); // swap buffers
	}
}
