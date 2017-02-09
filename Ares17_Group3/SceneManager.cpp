#include "SceneManager.h"
#include <time.h>

using namespace std;


typedef std::pair<string, btRigidBody*> bodyID;

// this class still needs a lot of work
namespace SceneManager {

	Player *player;

	PointLight mainLight{
		glm::vec3(0.0f, 20.0f, 0.0f),

		ATTENUATION_CONST, ATTENUATION_LINEAR, ATTENUATION_QUAD,

		glm::vec3(AMBIENT_FACTOR,AMBIENT_FACTOR,AMBIENT_FACTOR),
		glm::vec3(DIFFUSE_FACTOR,DIFFUSE_FACTOR,DIFFUSE_FACTOR),
		glm::vec3(SPECULAR_FACTOR,SPECULAR_FACTOR,SPECULAR_FACTOR)
	};

	// Shaders
	//GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;

	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;

	unsigned int lastTime = 0, currentTime;
	enum pov { FIRST_PERSON, THIRD_PERSON };
	pov pointOfView = FIRST_PERSON;
	enum modes { PLAY, EDIT };
	modes mode = PLAY;
	enum bound { BOX, SPHERE };
	bound boundingType = BOX;

	// SHADOWS
	GLuint depthShaderProgram; //shader to create shadow cubemaps

	//////////////////
	/// FBO globals
	//////////////////
	GLuint depthMapFBO; // FBO
	GLuint depthCubemap;
	//GLuint depthMap;	// FBO texture
	const GLuint SHADOW_WIDTH = 1440, SHADOW_HEIGHT = 1440;
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	// near and far act the same as near and far plane for camera, but this is for shadows
	// If the level gets too big and you can see the edge of the shadowmapping, increase the far 
	// (or consider adding multiple lights / opting for a directional light shadowmap)
	GLfloat near_plane = 0.01f;
	GLfloat far_plane = 40.0f;
	//////////////////
	/// End
	//////////////////

	const char *testTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};

	const char *skyTexFiles[6] = {
		"star-skybox/sky_back.bmp", "star-skybox/sky_front.bmp", "star-skybox/sky_right.bmp", "star-skybox/sky_left.bmp", "star-skybox/sky_top.bmp", "star-skybox/sky_bot.bmp"
	};

	// Load modelTypes
	std::map<string, Model*> modelTypes;

	std::map<string, std::pair<string, glm::vec3>> models; //objType, <modelName, scale>
	std::map<string, btRigidBody*> bodies;	
	// TEST
	btRigidBody* playerBody;
	//	


	GLuint defaultTexture;
	GLuint groundTexture;

	glm::mat4 view;

	GLfloat pitch = 0.0f;
	GLfloat yaw = 0.0f;
	GLfloat roll = 0.0f;

	glm::vec3 eye(2.0f, 3.0f, -6.0f);
	glm::vec3 at(0.0f, 0.5f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);


	// Old movement methods, used in edit mode (?)
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
	
	// Speedforward/right allow moving the player in camera direction
	// Also remembered as that one time I managed to fit a conditional operator somewhere
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
			myfile << "BoundingName | ModelName | Positions | BoundingScale | ModelScale | Mass \t//numberOfBodies on first line\n";
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
				std::string boundingType;
				std::string modelName;
				btVector3 position;
				btVector3 boundingScale;
				glm::vec3 modelScale;
				float mass;
				//BoundingType | ModelName | Positions | BoundingScale, ModelScale, mASs
				if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					objType = "box";
					objType.append(std::to_string(boxNo));
					boundingType = bodies.find(objType)->first;
					modelName = models.find(objType)->second.first;
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					boundingScale = (((btBoxShape*)bodies.find(objType)->second->getCollisionShape())->getHalfExtentsWithMargin()) * 2; //*2 as its half
					modelScale = models.find(objType)->second.second;
					mass = bodies.find(objType)->second->getInvMass();
					myfile << boundingType << ",";
					myfile << modelName << ",";
					myfile << position.x() << ",";
					myfile << position.y() << ",";
					myfile << position.z() << ",";
					myfile << boundingScale.x() << ",";
					myfile << boundingScale.y() << ",";
					myfile << boundingScale.z() << ",";
					myfile << modelScale.x << ",";
					myfile << modelScale.y << ",";
					myfile << modelScale.z << ",";
					myfile << mass << ",";
					myfile << "\n";
					boxNo++;
					cout << "Boxes Saved\n";
				}
				else if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {// SPHERE_SHAPE_PROXYTYPE
					objType = "sphere";
					objType.append(std::to_string(sphereNo));
					boundingType = bodies.find(objType)->first;
					modelName = models.find(objType)->second.first;
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					float radius = ((btSphereShape*)bodies.find(objType)->second->getCollisionShape())->getRadius();
					modelScale = models.find(objType)->second.second;
					mass = bodies.find(objType)->second->getInvMass();
					myfile << boundingType << ",";
					myfile << modelName << ",";
					myfile << position.x() << ",";
					myfile << position.y() << ",";
					myfile << position.z() << ",";
					myfile << radius << ",";
					myfile << modelScale.x << ",";
					myfile << modelScale.y << ",";
					myfile << modelScale.z << ",";
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
				std::string key;
				std::string modelName;
				glm::vec3 position;
				glm::vec3 boundingScale;
				glm::vec3 modelScale;
				float radius;
				float mass;
				
				string asString;
				string numAsString;
				int variables = 2;
				if (!gotNumber) {
					getline(myfile, numAsString, '#');
					numberOfBodies = stoi(numAsString);
					gotNumber = true;
				}
				else if (bodyNo != numberOfBodies) {
					for (int i = 0; i <= variables; i++) {
						getline(myfile, asString, ',');
						if (i == 0) key = asString;
						else if (i == 1) modelName = asString;
						else {
							if (key.find("box") != std::string::npos) {
								variables = 11;
								float digit = stof(asString);
								if (i == 2) position.x = digit;
								if (i == 3) position.y = digit;
								if (i == 4) position.z = digit; 
								if (i == 5) boundingScale.x = digit;
								if (i == 6) boundingScale.y = digit;
								if (i == 7) boundingScale.z = digit;
								if (i == 8) modelScale.x = digit;
								if (i == 9) modelScale.y = digit;
								if (i == 10) modelScale.z = digit;
								if (i == 11) mass = digit;
							}
							else if (key.find("sphere") != std::string::npos) {
								variables = 9;
								float digit = stof(asString);
								if (i == 2) position.x = digit;
								if (i == 3) position.y = digit;
								if (i == 4) position.z = digit;
								if (i == 5) radius = digit;
								if (i == 6) modelScale.x = digit;
								if (i == 7) modelScale.y = digit;
								if (i == 8) modelScale.z = digit;
								if (i == 9) mass = digit;
							}
						}//else
					}//for loop
					bodyNo++;
				}
				if (key.find("box") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::pair<string, glm::vec3>>(key, std::make_pair(modelName, modelScale)));
					cout << "Box Added\n";
				}
				else if (key.find("sphere") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addSphere(radius, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::pair<string, glm::vec3>>(key, std::make_pair(modelName, modelScale)));
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

	void initmodelTypes() {
		modelTypes.insert(std::pair<string, Model*>("nanosuit", new Model("Models/Nanosuit/nanosuit.obj")));
		modelTypes.insert(std::pair<string, Model*>("pistol", new Model("Models/Weapons/Socom pistol.obj")));
		modelTypes.insert(std::pair<string, Model*>("plasmacutter", new Model("Models/Weapons/Plasmacutter/DYIPlasmcutter.obj")));
		modelTypes.insert(std::pair<string, Model*>("cube", new Model("Models/cube.obj")));
		modelTypes.insert(std::pair<string, Model*>("sphere", new Model("Models/sphere.obj")));
		modelTypes.insert(std::pair<string, Model*>("car", new Model("Models/Car/model.obj")));
		modelTypes.insert(std::pair<string, Model*>("house", new Model("Models/House/houselow.obj")));
		modelTypes.insert(std::pair<string, Model*>("robot", new Model("Models/Robot/Roboto.obj")));
	}
	
	void insertBounding(std::string modelName, glm::vec3 modelScale, glm::vec3 boundingScale, float radius) {
		glm::vec3 position(moveForward(player->getPosition(), yaw, 0.5f));
		float mass = 0.5;
		std::string key;
		
		if (boundingType == BOX) {
			key = "box";
			key.append(to_string(boxNo));
			bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, position.x, position.y, position.z, mass)));
			cout << key << " Added\n";
			boxNo++;
		}
		if (boundingType == SPHERE) {
			key = "sphere";
			key.append(to_string(sphereNo));
			bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addSphere(radius, position.x, position.y, position.z, mass)));
			cout << key << "Added\n";
			sphereNo++;
		}
		bodies[key]->setActivationState(DISABLE_DEACTIVATION);
		models.insert(std::pair<string, std::pair<string, glm::vec3>>(key, make_pair(modelName, modelScale)));
	}

	// +++!
	btPairCachingGhostObject* ghostObject;

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
		playerBody->setAngularFactor(0); // Doesn't fall sideways
		bt_manager->addToWorld(playerBody);
		playerBody->setActivationState(DISABLE_DEACTIVATION);
		playerBody->setFriction(8);

		// Now ghost
		ghostObject = new btPairCachingGhostObject();								// create object
		ghostObject->setCollisionShape(playerShape);								// set shape
		ghostObject->setWorldTransform(t);											// set world transform	
		ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);  // disable collision response // could also add CF_CHARACTER_OBJECT // If I knew what that flag did...
																					
		bt_manager->addGhostToWorld(ghostObject);

	}

	void  findCollision(btPairCachingGhostObject* ghostObject) { // ignore player?
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray =
			ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; ++i){
			manifoldArray.clear();
			const btBroadphasePair& pair = pairArray[i];
			btBroadphasePair* collisionPair = bt_manager->findWorldPair(pair);
			if (!collisionPair) continue;
			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
			for (int j = 0; j < manifoldArray.size(); j++)	{
				btPersistentManifold* manifold = manifoldArray[j];
				bool isFirstBody = manifold->getBody0() == ghostObject;
				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);
				for (int p = 0; p < manifold->getNumContacts(); ++p)	{
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.f)	{
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						// <START>  handle collisions here
						cout << "Colliding with something";
						player->setState(ON_GROUND);
						//  <END>   handle collisions here
					}
				}
			}
		}
	}

	void init(void) {

		//shaderProgram = ShaderManager::initShaders("Shaders/phong-tex.vert", "Shaders/phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("Shaders/textured.vert", "Shaders/textured.frag");
		modelProgram = ShaderManager::initShaders("Shaders/modelLoading.vert", "Shaders/modelLoading.frag");
		//+++
		depthShaderProgram = ShaderManager::initShaders("Shaders/simpleShadowMap.vert", "Shaders/simpleShadowMap.frag", "Shaders/simpleShadowMap.gs");
		//+++
		bt_manager = new btShapeManager();

		initmodelTypes();

		defaultTexture = loadBitmap::loadBitmap("wall.bmp");
		groundTexture = loadBitmap::loadBitmap("terrain.bmp");

		initPlayer(1.0f, 1.5f, 40.0f);
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
	
	int objectID = 0;
	/*std::string bodyParser() {
		std::string lastKey = "";
		std::string object = "";
		if (addmode == BOX)
			object = "box";
		else if (addmode == SPHERE)
			object = "sphere";
		for (const auto& id_pair : bodies) {
			if (object.append(to_string(objectID)) == id_pair.first)
				lastKey = id_pair.first;
			else {
				if (addmode == BOX)
					object = "box";
				else if (addmode == SPHERE)
					object = "sphere";
				lastKey = object.append(to_string(0));
			}
		}
		return lastKey;
	}*/

	bool leftClick = false;
	bool rightClick = false;
	float coolDownOfGun = 0.5; //wait between shots
	bool shiftPressed = true;
	std::string currentModel = "nanosuit";
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
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
				if (coolDownOfGun <= 0.0f) {
					leftClick = true;
					coolDownOfGun = 0.5f;
				}
			}
			if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
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
			mode = PLAY;
		}
		else if (keys[SDL_SCANCODE_RIGHTBRACKET]) {
			mode = EDIT;
		}

		float increase;
		if (mode == PLAY) {
			if (player->getState() == ON_GROUND)
				increase = 1.0f;
			else increase = 0.3f;
				if (keys[SDL_SCANCODE_W]) {
					playerBody->setLinearVelocity(speedForward(increase, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED))); // work in progress
				}
				else if (keys[SDL_SCANCODE_S]) {
					playerBody->setLinearVelocity(speedForward(-increase, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED))); // work in progress
				} 
				if (keys[SDL_SCANCODE_A]) {
					//player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
					playerBody->setLinearVelocity(speedRight(-increase, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED))); // work in progress
				}
				else if (keys[SDL_SCANCODE_D]) {
					//player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
					playerBody->setLinearVelocity(speedRight(increase, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED))); // work in progress
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
		else if (mode == EDIT) {
			
			std::string lastKey;
			if (boundingType == BOX) {
				lastKey = std::string("box").append(to_string(boxNo-1));
			}
			else if (boundingType == SPHERE) {
				lastKey = std::string("sphere").append(to_string(sphereNo - 1));
			}

			btVector3 lastObject = bodies[lastKey]->getWorldTransform().getOrigin();
			btVector3 playerPos = playerBody->getWorldTransform().getOrigin();

			btTransform t;
			t.setIdentity();

			if (leftClick == true && pointOfView == FIRST_PERSON) {
				insertBounding(currentModel, glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.2, 0.2, 0.2), 0.2);	
				leftClick = false;
			}

			if (keys[SDL_SCANCODE_W])
				player->setPosition(moveForward(player->getPosition(), yaw, 0.1f));
			else if (keys[SDL_SCANCODE_S]) 
				player->setPosition(moveForward(player->getPosition(), yaw, -0.1f));
			if (keys[SDL_SCANCODE_A])
				player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
			else if (keys[SDL_SCANCODE_D])
				player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
			if (keys[SDL_SCANCODE_LSHIFT]) {
				shiftPressed = false;
			}
			if (keys[SDL_SCANCODE_LCTRL]) {
				shiftPressed = true;
			}

			if (keys[SDL_SCANCODE_O]) {
				writeFile();
			}

			if (keys[SDL_SCANCODE_KP_DIVIDE]){
				boundingType = BOX;
				std::cout << "Bounding Type: BOX" << std::endl;
			}
			if (keys[SDL_SCANCODE_KP_MULTIPLY]){
				boundingType = SPHERE;
				std::cout << "Bounding Type: SPHERE" << std::endl;
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

			if (keys[SDL_SCANCODE_KP_7]) {
				btQuaternion s = btQuaternion(btScalar(-1.0), btScalar(0.0), btScalar(0.0), btScalar(0.0));
				bodies[lastKey]->getWorldTransform().setRotation(s);
			}
			if (keys[SDL_SCANCODE_KP_9]) {
				btQuaternion s = btQuaternion(btScalar(1.0), btScalar(0.0), btScalar(0.0), btScalar(0.0));
				bodies[lastKey]->getWorldTransform().setRotation(s);
			}
			
			if (keys[SDL_SCANCODE_KP_PLUS]) {
				if (objectID > modelTypes.size() - 1)
					objectID = modelTypes.size() - 1;
				else
					objectID++;
				int i = 0;
				for (const auto& id_pair : modelTypes) {
					if (i == objectID)
						currentModel = id_pair.first;
					i++;
				}
				std::cout << currentModel << " selected" << std::endl;
			}
			if (keys[SDL_SCANCODE_KP_MINUS]) {
				if (objectID < 0)
					objectID = 0;
				else
					objectID--;
				int i = 0;
				for (const auto& id_pair : modelTypes) {
					if (i == objectID)
						currentModel = id_pair.first;
					i++;
				}
				std::cout << currentModel << " selected" << std::endl;
			}
			
			float scaling = 0.0005;
			if (shiftPressed) {
				if (keys[SDL_SCANCODE_KP_1]) {
					models[lastKey].second.x += scaling;
				}
				if (keys[SDL_SCANCODE_KP_2]) {
					models[lastKey].second.y += scaling;
				}
				if (keys[SDL_SCANCODE_KP_3]) {
					models[lastKey].second.z += scaling;
					/*float scale = lastObjectScale.z();
					scale += scaling;
					btBoxShape* box = new btBoxShape(btVector3(lastObjectScale.x(), lastObjectScale.y(), scale));
					bodies[lastKey]->setCollisionShape(box);*/
				}
				if (keys[SDL_SCANCODE_KP_0]) {
					models[lastKey].second.x += scaling;
					models[lastKey].second.y += scaling;
					models[lastKey].second.z += scaling;
				}
			}
			if (!shiftPressed) {
				if (keys[SDL_SCANCODE_KP_1]) {
					models[lastKey].second.x -= scaling;
				}
				if (keys[SDL_SCANCODE_KP_2]) {
					models[lastKey].second.y -= scaling;
				}
				if (keys[SDL_SCANCODE_KP_3]) {
					models[lastKey].second.z -= scaling;
					/*float scale = lastObjectScale.z();
					scale += scaling;
					btBoxShape* box = new btBoxShape(btVector3(lastObjectScale.x(), lastObjectScale.y(), scale));
					bodies[lastKey]->setCollisionShape(box);*/
				}
				if (keys[SDL_SCANCODE_KP_0]) {
					models[lastKey].second.x -= scaling;
					models[lastKey].second.y -= scaling;
					models[lastKey].second.z -= scaling;
				}
			}
		}


		if (keys[SDL_SCANCODE_R]) {
			//cout << getLinearVelocityInBodyFrame(playerBody).x() << " " << getLinearVelocityInBodyFrame(playerBody).y() << " " << getLinearVelocityInBodyFrame(playerBody).z() << "\n";
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y + 0.1, player->getPosition().z));
		}
		else if (keys[SDL_SCANCODE_F]) {
			player->setPosition(glm::vec3(player->getPosition().x, player->getPosition().y - 0.1, player->getPosition().z));
		}
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

	void renderObject(glm::mat4 proj, Model *modelData, glm::vec3 pos, glm::vec3 scale, GLuint shader, GLuint texture, float rotate) {
		glm::mat4 model;
		model = glm::translate(model, pos);
		//model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(rotate * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, scale);	// It's a bit too big for our scene, so scale it down
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// for gun]
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

    glBindTexture(GL_TEXTURE_2D, 0);
	}

	/*void renderWeapon(glm::mat4 proj, Model *modelData, GLuint shader) {
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
		
		if (rightClick) {
			glm::vec3 gun_pos(0.0f, -2.0f, -5.0f);
			float Y_axisRotation = -85.0f*DEG_TO_RADIAN;
			float Z_axisRotation = -25.0f*DEG_TO_RADIAN;
			model = glm::translate(model, gun_pos);
			model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
		//	model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		} 
		//	else {
		glm::vec3 gun_pos(2.5f, -2.5f, -5.0f);
		float Y_axisRotation = 30.0f*DEG_TO_RADIAN;
		model = glm::translate(model, gun_pos);
		//model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate in y axis
		//model = glm::rotate(model, Z_axisRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate in z axis
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
//	}
		//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// It's a bit too big for our scene, so scale it down

		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

		//	mvStack.pop();
			//	glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 
		glDepthMask(GL_TRUE);
	}*/

	void renderWeapon(glm::mat4 proj, Model *modelData, GLuint shader) {
		glm::mat4 model;
		glm::vec3 gunPos = moveForward(glm::vec3(player->getPosition().x, player->getPosition().y-0.15, player->getPosition().z), yaw, 0.2);
		gunPos = moveRight(gunPos, yaw, 0.2);
		model = glm::translate(model, gunPos);
		float Y_axisRotation = 30.0f*DEG_TO_RADIAN;
		model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(pitch*DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, Y_axisRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	//	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
																//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// for gun
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);
	}

	void updatePlayer() {
		btTransform t;
		t.setIdentity();
		playerBody->getMotionState()->getWorldTransform(t);
		//	playerBody->getMotionState();
		btVector3 pos = t.getOrigin();
		player->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
		ghostObject->setWorldTransform(t);
		//cout << getLinearVelocityInBodyFrame(playerBody).y();
	}

	float gameTime() {
		currentTime = clock();

		unsigned int dt = currentTime - lastTime;
		float dt_secs = (float)dt / 1000;
		if (dt_secs > 0.017) dt_secs = 0.017; // first value is off ( 5.5~)

		lastTime = currentTime;

		return dt_secs;
	}

	void update(SDL_Window * window, SDL_Event sdlEvent) {
		controls(window, sdlEvent);

		float dt_secs = gameTime();
		coolDownOfGun -= dt_secs;

		// 18/01
		updatePlayer();

		bt_manager->update();
		if (player->getState() == JUMPING)
			findCollision(ghostObject);

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
		}

	}

	//function that passes all light positions and properties to the shader

	void pointLight(GLuint shader) {
		GLuint uniformIndex = glGetUniformLocation(shader, "viewPos");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(player->getPosition()));
		uniformIndex = glGetUniformLocation(shader, "pointLight.position");
		glUniform3f(uniformIndex, mainLight.position.x, mainLight.position.y, mainLight.position.z);
		uniformIndex = glGetUniformLocation(shader, "pointLight.ambient");
		glUniform3f(uniformIndex, mainLight.ambient.r, mainLight.ambient.g, mainLight.ambient.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.diffuse");
		glUniform3f(uniformIndex, mainLight.diffuse.r, mainLight.diffuse.g, mainLight.diffuse.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.specular");
		glUniform3f(uniformIndex, mainLight.specular.r, mainLight.specular.g, mainLight.specular.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.constant");
		glUniform1f(uniformIndex, mainLight.att_constant);
		uniformIndex = glGetUniformLocation(shader, "pointLight.linear");
		glUniform1f(uniformIndex, mainLight.att_linear);
		uniformIndex = glGetUniformLocation(shader, "pointLight.quadratic");
		glUniform1f(uniformIndex, mainLight.att_quadratic);
	}

	void pointShadow(GLuint shader) {
		glm::mat4 shadowProj = glm::perspective(float(90.0f*DEG_TO_RADIAN), aspect, near_plane, far_plane); //perspective projection is the best suited for this
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(mainLight.position, mainLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		for (int k = 0; k < 6; ++k)
			glUniformMatrix4fv(glGetUniformLocation(shader, ("shadowMatrices[" + std::to_string(k) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[k]));
	}

	void renderWorldObjects(GLuint shader, glm::mat4 projection) {

		// PLAYER capsule
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		bt_manager->renderCapsule(playerBody, view, projection, modelTypes["sphere"], shader, defaultTexture);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		///+++++++++++++++

		int i = 0;
		for (const auto& id_pair : bodies) {
			// First = name / key
			id_pair.first; // string
			btVector3 p = bodies[id_pair.first]->getWorldTransform().getOrigin();
			glm::vec3 position = glm::vec3(p.x(), p.y(), p.z());
		//	btQuaternion test = bodies[id_pair.first]->getWorldTransform().getRotation();
	//		test.getAngle();

			if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				bt_manager->renderBox(bodies[id_pair.first], view, projection, modelTypes["cube"], shader, groundTexture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				renderObject(projection, modelTypes[models[id_pair.first].first], position, models[id_pair.first].second, shader, groundTexture, bodies[id_pair.first]->getWorldTransform().getRotation().getX());
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				bt_manager->renderSphere(bodies[id_pair.first], view, projection, modelTypes["sphere"], shader, defaultTexture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				renderObject(projection, modelTypes[models[id_pair.first].first], position, models[id_pair.first].second, shader, defaultTexture, bodies[id_pair.first]->getWorldTransform().getRotation().getX());
			}
			i++;
		}
		///+++++++++++++++
		// RENDERING modelTypes

		if (pointOfView == THIRD_PERSON)
			renderObject(projection, modelTypes["nanosuit"], glm::vec3(player->getPosition().x, player->getPosition().y-1.75, player->getPosition().z), glm::vec3(0.2,0.2,0.2), shader, 0, -yaw);
  // rip robot
    //		renderObject(projection, modelTypes["robot"], glm::vec3(4.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2), shader);

		if (pointOfView == FIRST_PERSON)
			renderWeapon(projection, modelTypes["plasmacutter"], shader);
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
		glUniform1f(uniformIndex, far_plane);
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

	void draw(SDL_Window * window) { //, int fps) { // fps counter; 4 of 5
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

				skybox->renderSkybox(projection, view, modelTypes["cube"]);
				// normal rendering
				renderShadowScene(projection, view, modelProgram, false); // render normal scene from normal point of view
				// fps counter; 5 of 5
				//h_manager->renderToHud(fps, texturedProgram, modelTypes["cube"], glm::vec3(-0.9f, 0.9f, 0.9f));
			}
			glDepthMask(GL_TRUE);
		}
		SDL_GL_SwapWindow(window); // swap buffers
	}
}
