#include "SceneManager.h"

using namespace std;

#define SPEED_CAP_XZ 10.0
#define SPEED_CAP_Y 10.0

typedef std::pair<string, btRigidBody*> bodyID;

// this class still needs a lot of work
namespace SceneManager {

	Player *player;
	glm::vec3 playerScale(1.0, 2.8, 1.0);

	// Shaders
	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;

	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;

	float SCREENWIDTH = 800.0f;
	float SCREENHEIGHT = 600.0f;
	unsigned int lastTime = 0, currentTime;
	enum pov { FIRST_PERSON, THIRD_PERSON };
	pov pointOfView = FIRST_PERSON;
	enum mode { PLAY_MODE, EDIT_MODE };
	mode editmode = PLAY_MODE;


	// SHADOWS
	GLuint depthShaderProgram; //shader to create shadow cubemaps

	//////////////////
	/// FBO globals
	//////////////////
	GLuint depthMapFBO; // FBO
	GLuint depthCubemap;
	//GLuint depthMap;	// FBO texture
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat near = 0.01f;
	GLfloat far = 25.0f;
	//////////////////
	/// End
	//////////////////


	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	const char *skyTexFiles[6] = {
		"star-skybox/sky_back.bmp", "star-skybox/sky_front.bmp", "star-skybox/sky_right.bmp", "star-skybox/sky_left.bmp", "star-skybox/sky_top.bmp", "star-skybox/sky_bot.bmp"
	};

	// Load models
	Model *nanosuit;
	Model *pistol;
	Model *sphere;
	Model *cube;
	GLuint defaultTexture;

	glm::mat4 view;

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
	//glm::vec4 lightPos(0.0, 5.0, 0.0, 1.0);
	glm::vec3 lightPos(0.0, 5.0, 0.0);

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

	MeshManager::materialStruct purpleMaterial = {
		{ 0.6f, 0.4f, 1.0f, 1.0f }, // ambient
		{ 0.6f, 0.4f, 1.0f, 1.0f }, // diffuse
		{ 0.6f, 0.4f, 1.0f, 1.0f }, // specular
		2.0f  // shininess
	};

	MeshManager::materialStruct defaultMaterial = {
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // ambient
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // diffuse
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // specular
		2.0f  // shininess
	};

	std::map<string, btRigidBody*> bodies;	
	
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(yaw*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(yaw*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(yaw*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(yaw*DEG_TO_RADIAN));
	}

	static btVector3 getLinearVelocityInBodyFrame(btRigidBody* body)
	{
		return(body->getWorldTransform().getBasis().transpose() *
			body->getLinearVelocity());
	}

	btVector3 speedForward(GLfloat _speed, GLfloat angle, bool concurrent) {
		btVector3 speed = getLinearVelocityInBodyFrame(playerBody);

		if (!concurrent && speed.absolute().x() <= SPEED_CAP_XZ && speed.absolute().z() <= SPEED_CAP_XZ)
			speed = btVector3(speed.x() + _speed*std::sin(angle*DEG_TO_RADIAN), speed.y(), speed.z() - _speed*std::cos(angle*DEG_TO_RADIAN));
		else if (concurrent)
			speed = btVector3(speed.x() + (speed.absolute().x() > SPEED_CAP_XZ ? 0 : _speed*std::sin(angle*DEG_TO_RADIAN)), speed.y(), speed.z() - (speed.absolute().z() > SPEED_CAP_XZ ? 0 : _speed*std::cos(angle*DEG_TO_RADIAN)));
		return speed;
	}

	btVector3 speedRight(GLfloat _speed, GLfloat angle, bool concurrent) {

		//playerBody->getVelocityInLocalPoint();
		btVector3 speed = getLinearVelocityInBodyFrame(playerBody);
		if (!concurrent && speed.absolute().x() <= SPEED_CAP_XZ && speed.absolute().z() <= SPEED_CAP_XZ)
			speed = btVector3(speed.x() + _speed*std::cos(angle*DEG_TO_RADIAN), speed.y(), speed.z() + _speed*std::sin(angle*DEG_TO_RADIAN));
		else if (concurrent)
			speed = btVector3(speed.x() + (speed.absolute().x() > SPEED_CAP_XZ ? 0 : _speed*std::cos(angle*DEG_TO_RADIAN)), speed.y(), speed.z() + (speed.absolute().z() > SPEED_CAP_XZ ? 0 : _speed*std::sin(angle*DEG_TO_RADIAN)));
		return speed;
	}

	btVector3 jump(GLfloat _speed) {
		btVector3 speed = getLinearVelocityInBodyFrame(playerBody);
		if (player->getState() != JUMPING) {
			speed = btVector3(speed.x(), speed.y() + _speed, speed.z());
			player->setState(JUMPING);
		}
		return speed;
	}
	
	int boxNo = 0;
	int sphereNo = 0;

	void writeFile() {
		// writing on a text file
		ofstream myfile("bodies.txt");
		if (myfile.is_open())
		{
			myfile << "Name | Positions | Scale\t//numberOfBodies on first line\n";
			myfile << bodies.size() << "#\n";

			bodyID id_pair;
			int boxNo = 0;
			int sphereNo = 0;
			for (const auto& id_pair : bodies) {
				// First = name / key
				id_pair.first; // string

							   // second = body
				id_pair.second->getWorldTransform().getOrigin(); // rigidBody

				std::string objType;
				string modelName;
				btVector3 position;
				btVector3 scale;
				float mass;
				if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					objType = "box";
					objType.append(std::to_string(boxNo));
					modelName = bodies.find(objType)->first;
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					scale = (((btBoxShape*)bodies.find(objType)->second->getCollisionShape())->getHalfExtentsWithMargin()) * 2; //*2 as its half
					mass = bodies.find(objType)->second->getInvMass();
					myfile << modelName << ",";
					myfile << scale.x() << ",";
					myfile << scale.y() << ",";
					myfile << scale.z() << ",";
					myfile << position.x() << ",";
					myfile << position.y() << ",";
					myfile << position.z() << ",";
					myfile << mass << ",";
					myfile << "\n";
					boxNo++;
					cout << "Boxes Saved\n";
				}
				else if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {// SPHERE_SHAPE_PROXYTYPE
					objType = "sphere";
					objType.append(std::to_string(sphereNo));
					modelName = bodies.find(objType)->first;
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					float radius = ((btSphereShape*)bodies.find(objType)->second->getCollisionShape())->getRadius();
					mass = bodies.find(objType)->second->getInvMass();
					myfile << modelName << ",";
					myfile << radius << ",";
					myfile << position.x() << ",";
					myfile << position.y() << ",";
					myfile << position.z() << ",";
					myfile << mass << ",";
					myfile << "\n";
					sphereNo++;
					cout << "Spheres Saved\n";
				}
			}
			myfile.close();
		}
		else cout << "Unable to open file";
	}

	void readFile() {
		// reading a text file
		string line;
		ifstream myfile("bodies.txt");
		if (myfile.is_open())
		{
			int numberOfBodies;
			bool gotNumber = false;
			int bodyNo = 0;
			while (getline(myfile, line, '\n')) //delimiter is '\n' in this
			{
				glm::vec3 position;
				glm::vec3 scale;
				float radius;
				float mass;
				string key;
				string asString;
				string numAsString;
				int variables = 1;
				if (!gotNumber) {
					getline(myfile, numAsString, '#');
					numberOfBodies = stoi(numAsString);
					gotNumber = true;
				}
				else if (bodyNo != numberOfBodies) {
					for (int i = 0; i <= variables; i++) {
						getline(myfile, asString, ',');
						if (i == 0) key = asString;
						else {
							if (key.find("box") != std::string::npos) {
								variables = 7;
								float digit = stof(asString);
								if (i == 1) scale.x = digit;
								if (i == 2) scale.y = digit;
								if (i == 3) scale.z = digit;
								if (i == 4) position.x = digit;
								if (i == 5) position.y = digit;
								if (i == 6) position.z = digit;
								if (i == 7) mass = digit;
							}
							else if (key.find("sphere") != std::string::npos) {
								variables = 5;
								float digit = stof(asString);
								if (i == 1) radius = digit;
								if (i == 2) position.x = digit;
								if (i == 3) position.y = digit;
								if (i == 4) position.z = digit;
								if (i == 5) mass = digit;
							}
						}//else
					}//for loop
					bodyNo++;
				}
				if (key.find("box") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addBox(scale.x, scale.y, scale.z, position.x, position.y, position.z, mass)));
					cout << "Box Added\n";
				}
				else if (key.find("sphere") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addSphere(radius, position.x, position.y, position.z, mass)));
					cout << "Sphere Added\n";
				}

			}//while loop
		}
		else cout << "\nMission failed. We'll get em next time. \n. Unable to open file";
	}

	void initBoxes() {
		readFile();

		bodyID id_pair;

		std::string key;
		for (const auto& id_pair : bodies) {
			id_pair.first; // string
			id_pair.second->getWorldTransform().getOrigin(); // rigidBody

			if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
				key = "box";
				key.append(to_string(boxNo));
				bodies[key]->setActivationState(DISABLE_DEACTIVATION);
				boxNo++;
			}
			else if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
				key = "sphere";
				key.append(to_string(sphereNo));
				bodies[key]->setActivationState(DISABLE_DEACTIVATION);
				sphereNo++;
			}
		}
		// NOTE : should probably use this
		/*
		body->setCollisionFlags( body->getCollisionFlags() |
		btCollisionObject::CF_KINEMATIC_OBJECT);
		*/
	}

	void insertBox() {
		glm::vec3 position(moveForward(player->getPosition(), yaw, 0.5f));
		glm::vec3 scale(0.5, 0.5, 0.5);
		float mass = 0.5;
		std::string key = "box";

		key.append(to_string(boxNo));
		bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addBox(scale.x, scale.y, scale.z, position.x, position.y, position.z, mass)));
		cout << "Box Added\n";
		bodies[key]->setActivationState(DISABLE_DEACTIVATION);
		boxNo++;
	}

	// TEST
	btRigidBody* playerBody;
	//

	void initPlayer(float rad, float height, float mass) {
		player = new Player(eye);

		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(player->getPosition().x, player->getPosition().y, player->getPosition().z));
		btCapsuleShape* playerShape = new btCapsuleShape(rad, height);
		btVector3 inertia(0, 0, 0);
		if (mass != 0.0)
			playerShape->calculateLocalInertia(mass, inertia);
		btMotionState* motion = new btDefaultMotionState(t);

		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, playerShape, inertia);

		playerBody = new btRigidBody(info);
		playerBody->setAngularFactor(0); // ? // Doesn't fall sideways
		bt_manager->addToWorld(playerBody);
		playerBody->setActivationState(DISABLE_DEACTIVATION);
		playerBody->setFriction(8);

		// btRigidBody::setAngularFactor // to 0
		// +++++

	}

	//Collision callback
	// id, index -> triangle mesh
	// flag - 1 static, 2 kinematic - 4 no contact response (through object....)
	// no contact response e.g. -> sphere of action of a lever (we know player is inside aka colliding, but we don't move him around)
	// 8 material callback
	bool callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2)
	{
		// add collision flag to rest of flags
		// playerBody->setCollisionFlags(playerBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK); // add
		// SEE GLOBAL ON TOP
		std::cout << "collision" << std::endl;
		return false;
	}

	void init(void) {

		shaderProgram = ShaderManager::initShaders("phong-tex.vert", "phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("textured.vert", "textured.frag");
		modelProgram = ShaderManager::initShaders("modelLoading.vert", "modelLoading.frag");
		//+++
		depthShaderProgram = ShaderManager::initShaders("simpleShadowMap.vert", "simpleShadowMap.frag", "simpleShadowMap.gs");

		//+++
		bt_manager = new btShapeManager();

		nanosuit = new Model("Nanosuit/nanosuit.obj");
		pistol = new Model("CHOO/Socom pistol.obj");
		cube = new Model("cube.obj");
		defaultTexture = loadBitmap::loadBitmap("wall.bmp");
		sphere = new Model("sphere.obj"); // THIS MODEL IS TERRIBLE
		MeshManager::setLight(shaderProgram, testLight);
		MeshManager::setMaterial(shaderProgram, greenMaterial);

		initPlayer(1.0f, 1.5f, 80.0f);
		initBoxes();
		h_manager = new hudManager();
		skybox = new Skybox(skyTexFiles);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		////////////////////
		/// FBO for shadows
		/////////////////////
		glGenFramebuffers(1, &depthMapFBO);
		// Create depth cubemap texture
		//	GLuint depthCubemap;
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// Attach cubemap as depth map FBO's color buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/* Might be useful http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=2069
	btVector3 getVelocityAtWorldPosition(btRigidBody* body, const btVector3& worldposition, bool local)
	{
		/* That's the 'localpoint'
		const btVector3 relpos = worldposition - body->getCenterOfMassPosition();
		/* That's velocity in world space
		const btVector3 wvel = body->getVelocityInLocalPoint(relpos);
		/* Now you can transform it in body local frame
		if (local) return(body->getWorldTransform().getBasis().transpose()*wvel);
		/* Or keep it in world frame
		return(wvel);

	} */

	//playerBody->setLinearVelocity(btVector3(0.0, 0.0, 0.0));

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

	float coolDownOfGun = 0.5; //wait between shots
	bool shiftPressed = true;
	//*****************************CONTROLS********************
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
			insertBox();
			leftClick = false;
		}

		if (sdlEvent.type == SDL_MOUSEBUTTONUP  && pointOfView == FIRST_PERSON) {
			leftClick = false;
			rightClick = false;
		}

		//KEYBOARD
		// To be changed ****

		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(0, 10, -10));
		btMotionState* motion = new btDefaultMotionState(t);
		motion->setWorldTransform(t);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_LEFTBRACKET]) {
			editmode = PLAY_MODE;
		}
		else if (keys[SDL_SCANCODE_RIGHTBRACKET]) {
			editmode = EDIT_MODE;
		}

		if (editmode == PLAY_MODE) {
			if (keys[SDL_SCANCODE_W]) {
				//	player->setPosition(moveForward(player->getPosition(), yaw, 0.1f));
				playerBody->setLinearVelocity(speedForward(1.0f, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED))); // work in progress
			}
			else if (keys[SDL_SCANCODE_S]) {
				//player->setPosition(moveForward(player->getPosition(), yaw, -0.1f));
				//bodies[1]->setLinearVelocity(btVector3(0.0, 5.0, 0.0));
				playerBody->setLinearVelocity(speedForward(-1.0f, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED))); // work in progress
			} //else player->setVelocity(glm::vec3(0.0, player->getVelocity().y, player->getVelocity().z));
			if (keys[SDL_SCANCODE_A]) {
				//player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
				playerBody->setLinearVelocity(speedRight(-1.0f, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED))); // work in progress
			}
			else if (keys[SDL_SCANCODE_D]) {
				//player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
				playerBody->setLinearVelocity(speedRight(1.0f, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED))); // work in progress
			}

			if (keys[SDL_SCANCODE_SPACE]) {
				playerBody->setLinearVelocity(jump(SPEED_CAP_Y));
			}

			if (keys[SDL_SCANCODE_KP_8])
				bodies["box1"]->setLinearVelocity(btVector3(5.0, 0.0, 0.0));
			if (keys[SDL_SCANCODE_KP_4])
				bodies["box1"]->setLinearVelocity(btVector3(0.0, 0.0, 5.0));
			if (keys[SDL_SCANCODE_KP_6])
				bodies["box1"]->setLinearVelocity(btVector3(0.0, 0.0, -5.0));
			if (keys[SDL_SCANCODE_KP_5])
				bodies["box1"]->setLinearVelocity(btVector3(-5.0, 0.0, 0.0));
		}
		else if (editmode == EDIT_MODE) {
			std::string lastKey;
			for (const auto& id_pair : bodies) {
				lastKey = id_pair.first;
			}

			btVector3 lastObject = bodies[lastKey]->getWorldTransform().getOrigin();
			btVector3 lastObjectScale = (((btBoxShape*)bodies[lastKey]->getCollisionShape())->getHalfExtentsWithMargin()) * 2;

			btVector3 playerPos = playerBody->getWorldTransform().getOrigin();

			btTransform t;
			t.setIdentity();

			if (keys[SDL_SCANCODE_W]) {
				player->setPosition(moveForward(player->getPosition(), yaw, 0.1f));
				/*glm::vec3 move = glm::vec3(moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, 0.1));
				t.setOrigin(btVector3(move.x, move.y, move.z));
				playerBody->setWorldTransform(t);*/
			}
			else if (keys[SDL_SCANCODE_S]) {
				player->setPosition(moveForward(player->getPosition(), yaw, -0.1f));
				/*glm::vec3 move = glm::vec3(moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, -0.1));
				t.setOrigin(btVector3(move.x, move.y, move.z));
				playerBody->setWorldTransform(t);*/
			}
			if (keys[SDL_SCANCODE_A]) {
				player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
				/*glm::vec3 move = glm::vec3(moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, -0.1));
				t.setOrigin(btVector3(move.x, move.y, move.z));
				playerBody->setWorldTransform(t);*/
			}
			else if (keys[SDL_SCANCODE_D]) {
				player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
				/*glm::vec3 move = glm::vec3(moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, 0.1));
				t.setOrigin(btVector3(move.x, move.y, move.z));
				playerBody->setWorldTransform(t);*/
			}

			if (keys[SDL_SCANCODE_LSHIFT]) {
				shiftPressed = false;
			}
			if (keys[SDL_SCANCODE_LCTRL]) {
				shiftPressed = true;
			}

			if (keys[SDL_SCANCODE_O]) {
				writeFile();
			}

			if (keys[SDL_SCANCODE_KP_8]) {
				glm::vec3 moveLeft = glm::vec3(moveForward(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, 0.1));
				t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
				bodies[lastKey]->setWorldTransform(t);
			}
			if (keys[SDL_SCANCODE_KP_4]) {
				glm::vec3 moveLeft = glm::vec3(moveRight(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, -0.1));
				t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
				bodies[lastKey]->setWorldTransform(t);
			}
			if (keys[SDL_SCANCODE_KP_6]) {
				glm::vec3 moveLeft = glm::vec3(moveRight(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, 0.1));
				t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
				bodies[lastKey]->setWorldTransform(t);
			}
			if (keys[SDL_SCANCODE_KP_5]) {
				glm::vec3 moveLeft = glm::vec3(moveForward(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, -0.1));
				t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
				bodies[lastKey]->setWorldTransform(t);
			}
		}

		if (keys[SDL_SCANCODE_R]) {
			//cout << getLinearVelocityInBodyFrame(playerBody).x() << " " << getLinearVelocityInBodyFrame(playerBody).y() << " " << getLinearVelocityInBodyFrame(playerBody).z() << "\n";
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y + 0.1, player->getPosition().z));
		}
		else if (keys[SDL_SCANCODE_F]) {
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y - 0.1, player->getPosition().z));
		}

		//update box
		// Simplistic; change
		//t.setIdentity();
		//t.setOrigin(btVector3(player->getPosition().x, player->getPosition().y, player->getPosition().z));
		//motion->setWorldTransform(t);
		//bodies[3]->setMotionState(motion);
/*
		if (keys[SDL_SCANCODE_SPACE] && player->getState() != JUMPING) {
			player->setVelocity(glm::vec3(player->getVelocity().x, 6.0f, player->getVelocity().z));
			player->setState(JUMPING);
		} */
		//++++
		if (keys[SDL_SCANCODE_M]) {
			//	cout << "Curiously cinnamon\n";
			bodies["box1"]->setLinearVelocity(btVector3(0.0, 5.0, 0.0));
		}
		if (keys[SDL_SCANCODE_N]) {

			//btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3(0, 10, -10));
			//btMotionState* motion = new btDefaultMotionState(t);
			cout << "Setting y\n";
			motion->setWorldTransform(t);
			bodies["box1"]->setMotionState(motion);
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

		//	if (keys[SDL_SCANCODE_3]) bodies[4]->setLinearVelocity(btVector3(0.0, 0.0, 4.0));
		if (keys[SDL_SCANCODE_ESCAPE]) {
			exit(0);
		}

		//	if (keys[SDL_SCANCODE_5])cout << bullet.size() << endl;
	}

	void renderObject(glm::mat4 proj, Model *modelData, glm::vec3 pos, GLuint shader) {
		//glUseProgram(modelProgram);
		//mvStack.push(mvStack.top());// push modelview to stack
	//	MeshManager::setLight(modelProgram, testLight);
	//	MeshManager::setMaterial(modelProgram, defaultMaterial);
	//	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
	//	MeshManager::setUniformMatrix4fv(modelProgram, "view", glm::value_ptr(view));
		//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
			// Draw the loaded model

		glm::mat4 model;
		model = glm::translate(model, pos);
		model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

		//mvStack.pop();
	}

	void renderWep(glm::mat4 proj, Model *modelData, GLuint shader) {
		//glUseProgram(modelProgram);
		glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
		//mvStack.push(glm::mat4(1.0));// push modelview to stack
									//		glCullFace(GL_BACK);
	//	MeshManager::setLight(modelProgram, testLight);
	//	MeshManager::setMaterial(modelProgram, redMaterial);
	//	MeshManager::setUniformMatrix4fv(modelProgram, "projection", glm::value_ptr(proj));
		MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(glm::mat4(1.0)));
		//	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
		// Draw the loaded model
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, defaultTexture);
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
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

		//	mvStack.pop();
			//	glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 
		glDepthMask(GL_TRUE);
	}

	void updatePlayer() {
		btTransform t;
		t.setIdentity();
		playerBody->getMotionState()->getWorldTransform(t);
		//	playerBody->getMotionState();
		btVector3 pos = t.getOrigin();
		player->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));

		//cout << getLinearVelocityInBodyFrame(playerBody).y();

/*
		btVector3 speed = getLinearVelocityInBodyFrame(playerBody);
		if (speed.x() >= SPEED_CAP_XZ)
			speed.setX(SPEED_CAP_XZ);
		if (speed.x() <= -SPEED_CAP_XZ)
			speed.setX(-SPEED_CAP_XZ);
		if (speed.y() >= SPEED_CAP_Y)
			speed.setY(SPEED_CAP_Y);
		//if (speed.y() > SPEED_CAP_Y)
		//	speed.setY(SPEED_CAP_Y);
		if (speed.z() >= SPEED_CAP_XZ)
			speed.setZ(SPEED_CAP_XZ);
		if (speed.z() <= -SPEED_CAP_XZ)
			speed.setZ(-SPEED_CAP_XZ);
		playerBody->setLinearVelocity(speed);
		*/
	}

	void update(SDL_Window * window, SDL_Event sdlEvent) {
		controls(window, sdlEvent);

		// 18/01
		updatePlayer();

		bt_manager->update();
		//world->stepSimulation(1/60.0); // 1 divided by frames per second
		// would need to delete dispatcher, collisionconfig, solver, world, broadphase in main
		// +++++
	}

	void camera() {
		if (pointOfView == FIRST_PERSON) {
			at = moveForward(player->getPosition(), yaw, 1.0f);
			at.y -= pitch;
			view = glm::lookAt(player->getPosition(), at, up);
			//mvStack.top() = glm::lookAt(player->getPosition(), at, up);
		}

		else {
			at = player->getPosition(); // look at player position
			eye = moveForward(at, pitch, -6.0f); // move behind him
			eye.y += pitch; // displacement determined by user interaction
			view = glm::lookAt(eye, at, up);
			//mvStack.top() = glm::lookAt(eye, at, up);
		}

		glm::vec4 tmp = view*glm::vec4(lightPos, 1.0f);
		//glm::vec4 tmp = mvStack.top()*lightPos;
		testLight.position[0] = tmp.x;
		testLight.position[1] = tmp.y;
		testLight.position[2] = tmp.z;
		MeshManager::setLightPos(modelProgram, glm::value_ptr(tmp));
	}

	//function that passes all light positions and properties to the shader
#define AMBIENT_FACTOR 0.8f
#define DIFFUSE_FACTOR 0.8f
#define SPECULAR_FACTOR 1.0f
#define ATTENUATION_CONST 0.2f
#define ATTENUATION_LINEAR 0.09f
#define ATTENUATION_QUAD 0.032f

	void pointLight(GLuint shader) {
		GLuint uniformIndex = glGetUniformLocation(shader, "viewPos");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(player->getPosition()));
		uniformIndex = glGetUniformLocation(shader, "pointLight.position");
		glUniform3f(uniformIndex, lightPos.x, lightPos.y, lightPos.z);
		uniformIndex = glGetUniformLocation(shader, "pointLight.ambient");
		glUniform3f(uniformIndex, AMBIENT_FACTOR, AMBIENT_FACTOR, AMBIENT_FACTOR);
		uniformIndex = glGetUniformLocation(shader, "pointLight.diffuse");
		glUniform3f(uniformIndex, DIFFUSE_FACTOR, DIFFUSE_FACTOR, DIFFUSE_FACTOR);
		uniformIndex = glGetUniformLocation(shader, "pointLight.specular");
		glUniform3f(uniformIndex, SPECULAR_FACTOR, SPECULAR_FACTOR, SPECULAR_FACTOR);
		uniformIndex = glGetUniformLocation(shader, "pointLight.constant");
		glUniform1f(uniformIndex, ATTENUATION_CONST);
		uniformIndex = glGetUniformLocation(shader, "pointLight.linear");
		glUniform1f(uniformIndex, ATTENUATION_LINEAR);
		uniformIndex = glGetUniformLocation(shader, "pointLight.quadratic");
		glUniform1f(uniformIndex, ATTENUATION_QUAD);
	}

	void pointShadow(GLuint shader) {
		glm::mat4 shadowProj = glm::perspective(float(90.0f*DEG_TO_RADIAN), aspect, near, far); //perspective projection is the best suited for this
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		for (int k = 0; k < 6; ++k)
			glUniformMatrix4fv(glGetUniformLocation(shader, ("shadowMatrices[" + std::to_string(k) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[k]));
	}

	void renderWorldObjects(GLuint shader, glm::mat4 projection) {

		// PLAYER capsule
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		bt_manager->renderCapsule(playerBody, view, projection, sphere, shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		///+++++++++++++++

		int i = 0;
		for (const auto& id_pair : bodies) {
			// First = name / key
			id_pair.first; // string

			if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
				bt_manager->renderBox(bodies[id_pair.first], view, projection, cube, shader);
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
				bt_manager->renderSphere(bodies[id_pair.first], view, projection, sphere, shader);
			}
			i++;
		}
		///+++++++++++++++
		// RENDERING MODELS
			if (pointOfView == THIRD_PERSON)
				renderObject(projection,nanosuit, glm::vec3(player->getPosition().x, player->getPosition().y-1.5, player->getPosition().z), shader);

			if (pointOfView == FIRST_PERSON)
				renderWep(projection, pistol, shader);
	}
	// main render function, sets up the shaders and then calls all other functions
	void renderShadowScene(glm::mat4 projection, glm::mat4 viewMatrix, GLuint shader, bool cubemap) {

		GLuint uniformIndex;
		glUseProgram(shader);
		pointLight(shader);
		// if cubemap translates into "if rendering to the depthmap"
		if (cubemap)
			pointShadow(shader);


		uniformIndex = glGetUniformLocation(shader, "far_plane");
		glUniform1f(uniformIndex, far);
		uniformIndex = glGetUniformLocation(shader, "viewPos");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(player->getPosition()));

		//similarly, if (!cubemap) refers to normal rendering
		if (!cubemap) {
			MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(projection));
			MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(viewMatrix));
			// material properties in this case roughly translate to textures
			uniformIndex = glGetUniformLocation(shader, "material.diffuse");
			glUniform1i(uniformIndex, 0);
			uniformIndex = glGetUniformLocation(shader, "material.specular");
			glUniform1i(uniformIndex, 0);
			glUniform1f(glGetUniformLocation(shader, "material.shininess"), 32.0f); //??
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, defaultTexture);
			// pass in the shadowmap
			uniformIndex = glGetUniformLocation(shader, "depthMap");
			glUniform1i(uniformIndex, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		}
		//draw normal scene
		renderWorldObjects(shader, projection);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}


	void draw(SDL_Window * window) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window
		glEnable(GL_CULL_FACE);
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glm::mat4 projection(1.0);
		GLfloat scale(1.0f); // just to allow easy scaling of complete scene
		//glm::mat4 modelview(1.0); // set base position for scene
		//mvStack.push(modelview);
		glDepthMask(GL_TRUE);
		view = glm::mat4(1.0);


		projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), SCREENWIDTH / SCREENHEIGHT, 0.1f, 150.0f);

		for (int pass = 0; pass < 2; pass++) {
			camera();
			if (pass == 0) {
				glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear FBO
				glClear(GL_DEPTH_BUFFER_BIT);

				renderShadowScene(projection, view, depthShaderProgram, true); // render using light's point of view and simpler shader program

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			} else {
				//Render to frame buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window
																	// clear the screen
				glEnable(GL_CULL_FACE);
				glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

				skybox->renderSkybox(projection, view, cube);
				// normal rendering
				renderShadowScene(projection, view, modelProgram, false); // render normal scene from normal point of view
			}


			//mvStack.pop();

			glDepthMask(GL_TRUE);
			
		}

		SDL_GL_SwapWindow(window); // swap buffers
	}
}
