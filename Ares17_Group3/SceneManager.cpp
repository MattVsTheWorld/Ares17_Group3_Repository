#include "SceneManager.h"

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
//	GLuint shaderProgram;
	GLuint texturedProgram;
	GLuint modelProgram;

	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;
	Projectile *projectile_manager; // !++!

	unsigned int lastTime = 0, currentTime;
	float dt_secs;
	enum pov { FIRST_PERSON, THIRD_PERSON };
	pov pointOfView = FIRST_PERSON;
	enum modes { PLAY, EDIT };
	modes mode = PLAY;
	enum bound { BOX, SPHERE, CAPSULE };
	bound boundingType = BOX;
	enum editStages { MODEL, BOUNDING };
	editStages stage = MODEL;

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
	GLfloat far_plane = 60.0f;
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
	map<string, Model*> modelTypes;

	map<std::string, std::tuple<string, glm::vec3, glm::vec3>> models; //objType, <modelName, scale>
	map<std::string, btRigidBody*> bodies;

	tuple<std::string, glm::vec3, glm::vec3, glm::vec3> temp[2]; //name, position, scale, rotation

	string currentModel = "nanosuit";
	string currentBounding = "box";

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

	glm::vec3 moveForward(glm::vec3 pos, GLfloat yaw, GLfloat pitch, GLfloat d) { // TODO: fix
		//cout << "x" << d*std::sin(yaw*DEG_TO_RADIAN) << endl;
		//cout << "y" << -d*std::sin(pitch) << endl;
		//cout << "z" << -d*std::cos(yaw*DEG_TO_RADIAN) << endl;
		return glm::vec3(pos.x + d*std::sin(yaw*DEG_TO_RADIAN), pos.y - d*std::sin(pitch), pos.z - d*std::cos(yaw*DEG_TO_RADIAN));
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
	int capsuleNo = 0;

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
			int capsuleNo = 0;
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
				glm::vec3 modelRotation;
				glm::vec3 boundingRotation;
				float mass;
				//BoundingType | ModelName | Positions | BoundingScale, ModelScale, mASs
				if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					objType = "box";
					objType.append(std::to_string(boxNo));
					boundingType = bodies.find(objType)->first;
					modelName = get<0>(models.find(objType)->second);
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					boundingScale = (((btBoxShape*)bodies.find(objType)->second->getCollisionShape())->getHalfExtentsWithMargin()) * 2; //*2 as its half
					boundingRotation = glm::vec3(bodies.find(objType)->second->getWorldTransform().getRotation().getX(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getY(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getZ());
					modelScale = get<1>(models.find(objType)->second);
					modelRotation = get<2>(models.find(objType)->second);
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
					myfile << modelRotation.x << ",";
					myfile << modelRotation.y << ",";
					myfile << modelRotation.z << ",";
					myfile << boundingRotation.x << ",";
					myfile << boundingRotation.y << ",";
					myfile << boundingRotation.z << ",";
					myfile << mass << ",";
					myfile << "\n";
					boxNo++;
					cout << "Boxes Saved\n";
				}
				else if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {// SPHERE_SHAPE_PROXYTYPE
					objType = "sphere";
					objType.append(std::to_string(sphereNo));
					boundingType = bodies.find(objType)->first;
					modelName = get<0>(models.find(objType)->second);
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					float radius = ((btSphereShape*)bodies.find(objType)->second->getCollisionShape())->getRadius();
					modelScale = get<1>(models.find(objType)->second);

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
				else if (id_pair.second->getCollisionShape()->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {// SPHERE_SHAPE_PROXYTYPE
					objType = "sphere";
					objType.append(std::to_string(sphereNo));
					boundingType = bodies.find(objType)->first;
					modelName = get<0>(models.find(objType)->second);
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					float radius = ((btCapsuleShape*)bodies.find(objType)->second->getCollisionShape())->getRadius();
					float height = ((btCapsuleShape*)bodies.find(objType)->second->getCollisionShape())->getHalfHeight() * 2;
					modelScale = get<1>(models.find(objType)->second);
					modelRotation = get<2>(models.find(objType)->second);
					boundingRotation = glm::vec3(bodies.find(objType)->second->getWorldTransform().getRotation().getX(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getY(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getZ());
					mass = bodies.find(objType)->second->getInvMass();
					myfile << boundingType << ",";
					myfile << modelName << ",";
					myfile << position.x() << ",";
					myfile << position.y() << ",";
					myfile << position.z() << ",";
					myfile << radius << ",";
					myfile << height << ",";
					myfile << modelScale.x << ",";
					myfile << modelScale.y << ",";
					myfile << modelScale.z << ",";
					myfile << modelRotation.x << ",";
					myfile << modelRotation.y << ",";
					myfile << modelRotation.z << ",";
					myfile << boundingRotation.x << ",";
					myfile << boundingRotation.y << ",";
					myfile << boundingRotation.z << ",";
					myfile << mass << ",";
					myfile << "\n";
					capsuleNo++;
					cout << "Capsule Saved\n";
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
				glm::vec3 boundingRotation;
				glm::vec3 modelScale;
				glm::vec3 modelRotation;
				float radius;
				float height;
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
								variables = 17;
								float digit = stof(asString);
								switch (i) {
								case 2:
									position.x = digit;
									break;
								case 3:
									position.y = digit;
									break;
								case 4:
									position.z = digit;
									break;
								case 5:
									boundingScale.x = digit;
									break;
								case 6:
									boundingScale.y = digit;
									break;
								case 7:
									boundingScale.z = digit;;
									break;
								case 8:
									modelScale.x = digit;
									break;
								case 9:
									modelScale.y = digit;
									break;
								case 10:
									modelScale.z = digit;
									break;
								case 11:
									modelRotation.z = digit;
									break;
								case 12:
									modelRotation.z = digit;
									break;
								case 13:
									modelRotation.z = digit;
									break;
								case 14:
									boundingRotation.z = digit;
									break;
								case 15:
									boundingRotation.z = digit;
									break;
								case 16:
									boundingRotation.z = digit;
									break;
								case 17:
									mass = digit;
									break;
								}
							}
							else if (key.find("sphere") != std::string::npos) {
								variables = 9;
								float digit = stof(asString);
								switch (i) {
								case 2:
									position.x = digit;
									break;
								case 3:
									position.y = digit;
									break;
								case 4:
									position.z = digit;
									break;
								case 5:
									radius = digit;
									break;
								case 6:
									modelScale.x = digit;
									break;
								case 7:
									modelScale.y = digit;
									break;
								case 8:
									modelScale.z = digit;
									break;
								case 9:
									mass = digit;
									break;
								}
							}
							else if (key.find("capsule") != std::string::npos) {
								variables = 10;
								float digit = stof(asString);
								switch (i) {
								case 2:
									position.x = digit;
									break;
								case 3:
									position.y = digit;
									break;
								case 4:
									position.z = digit;
									break;
								case 5:
									radius = digit;
									break;
								case 6:
									height = digit;
									break;
								case 7:
									modelScale.x = digit;
									break;
								case 8:
									modelScale.y = digit;
									break;
								case 9:
									modelScale.z = digit;
									break;
								case 11:
									modelRotation.z = digit;
									break;
								case 12:
									modelRotation.z = digit;
									break;
								case 13:
									modelRotation.z = digit;
									break;
								case 14:
									boundingRotation.z = digit;
									break;
								case 15:
									boundingRotation.z = digit;
									break;
								case 16:
									boundingRotation.z = digit;
									break;
								case 17:
									mass = digit;
									break;
								}
							}
						}//else
					}//for loop
					bodyNo++;
				}
				if (key.find("box") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, modelRotation)));
					cout << "Box Added\n";
				}
				else if (key.find("sphere") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addSphere(radius, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, glm::vec3(0., 0., 0.))));
					cout << "Sphere Added\n";
				}
				else if (key.find("capsule") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addCapsule(radius, height, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, modelRotation)));
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
			else if (id_pair.second->getCollisionShape()->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {
				key = "capsule";
				key.append(to_string(capsuleNo));
				bodies[key]->setActivationState(DISABLE_DEACTIVATION);
				capsuleNo++;
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
		modelTypes.insert(std::pair<string, Model*>("box", modelTypes["cube"]));
		modelTypes.insert(std::pair<string, Model*>("sphere", new Model("Models/sphere.obj")));
		modelTypes.insert(std::pair<string, Model*>("capsule", modelTypes["sphere"]));
		modelTypes.insert(std::pair<string, Model*>("car", new Model("Models/Car/model.obj")));
		modelTypes.insert(std::pair<string, Model*>("house", new Model("Models/House/houselow.obj")));
		modelTypes.insert(std::pair<string, Model*>("robot", new Model("Models/Robot/Roboto.obj")));
	}

	void insertBounding(std::string modelName, glm::vec3 modelScale, glm::vec3 boundingScale, glm::vec3 modelRotation, glm::vec3 boundingRotation) {
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
			bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addSphere(boundingScale.x, position.x, position.y, position.z, mass)));
			cout << key << "Added\n";
			sphereNo++;
		}
		if (boundingType == CAPSULE) {
			key = "capsule";
			key.append(to_string(capsuleNo));
			bodies.insert(std::pair<string, btRigidBody*>(key, bt_manager->addCapsule(boundingScale.x, boundingScale.y, position.x, position.y, position.z, mass)));
			cout << key << "Added\n";
			capsuleNo++;
		}
		bodies[key]->setActivationState(DISABLE_DEACTIVATION);
		models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3>>(key, make_tuple(modelName, modelScale, modelRotation)));
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
		int playerCollidesWith = COL_DEFAULT | COL_ENEMY;
		// body, group, mask
		bt_manager->addToWorld(playerBody, COL_PLAYER, playerCollidesWith);
		playerBody->setActivationState(DISABLE_DEACTIVATION);
		playerBody->setFriction(FRICTION);

		// Now ghost
		ghostObject = new btPairCachingGhostObject();								// create object
		ghostObject->setCollisionShape(playerShape);								// set shape
		ghostObject->setWorldTransform(t);											// set world transform	
		ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);  // disable collision response // could also add CF_CHARACTER_OBJECT // If I knew what that flag did...

		bt_manager->addGhostToWorld(ghostObject, COL_PLAYER, playerCollidesWith);

	}

	void findCollision(btPairCachingGhostObject* ghostObject) { // ignore player?
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray =
			ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; ++i) {
			manifoldArray.clear();
			const btBroadphasePair& pair = pairArray[i];
			btBroadphasePair* collisionPair = bt_manager->findWorldPair(pair);
			if (!collisionPair) continue;
			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
			for (int j = 0; j < manifoldArray.size(); j++) {
				btPersistentManifold* manifold = manifoldArray[j];
				bool isFirstBody = manifold->getBody0() == ghostObject;
				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);
				for (int p = 0; p < manifold->getNumContacts(); ++p) {
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.f) {
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						// <START>  handle collisions here
						cout << "Player colliding with something while jumping." << endl;
						player->setState(ON_GROUND);
						//  <END>   handle collisions here
					}
				}
			}
		}
	}

	void init(void) {

		//	shaderProgram = ShaderManager::initShaders("Shaders/phong-tex.vert", "Shaders/phong-tex.frag");
		texturedProgram = ShaderManager::initShaders("Shaders/textured.vert", "Shaders/textured.frag");
		modelProgram = ShaderManager::initShaders("Shaders/modelLoading.vert", "Shaders/modelLoading.frag");
		//+++
		depthShaderProgram = ShaderManager::initShaders("Shaders/simpleShadowMap.vert", "Shaders/simpleShadowMap.frag", "Shaders/simpleShadowMap.gs");
		//+++
		bt_manager = new btShapeManager();
		projectile_manager = new Projectile(bt_manager);

		initmodelTypes();

		defaultTexture = loadBitmap::loadBitmap("wall.bmp");
		groundTexture = loadBitmap::loadBitmap("terrain.bmp");

		initPlayer(1.0f, 1.5f, 40.0f);
		initBoxes();
		temp[0] = std::make_tuple(currentModel, at, glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.0f, yaw, 0.0f));
		temp[1] = std::make_tuple(currentBounding, std::get<1>(temp[0]), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.0f, yaw, 0.0f));

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
	//#define PITCHLOCK 3.0f //TODO: edit pitch lock
	void lockCamera()
	{
		//	cout << pitch << endl;
		if (pitch > PITCHLOCK)
			pitch = PITCHLOCK;
		if (pitch < -PITCHLOCK)
			pitch = -PITCHLOCK;
		if (yaw < 0.0)
			yaw += 360.0;
		if (yaw > 360.0)
			yaw -= 360;
	}

	int objectID = 0;
	/*std::string bodyParser() {
		std::string selectedObject.first = "";
		std::string object = "";
		if (addmode == BOX)
			object = "box";
		else if (addmode == SPHERE)
			object = "sphere";
		for (const auto& id_pair : bodies) {
			if (object.append(to_string(objectID)) == id_pair.first)
				selectedObject.first = id_pair.first;
			else {
				if (addmode == BOX)
					object = "box";
				else if (addmode == SPHERE)
					object = "sphere";
				selectedObject.first = object.append(to_string(0));
			}
		}
		return selectedObject.first;
	}*/

	bool leftClick = false;
	bool rightClick = false;
	bool creation = false;
	float coolDown = 0.0f; //wait between shots
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
				if (sdlEvent.type == SDL_MOUSEBUTTONDOWN && pointOfView == FIRST_PERSON) {
					if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
						if (coolDown <= 0.0f) {
							leftClick = true;
							coolDown = COOL_TIME;
							cout << "Attempting to shoot bullet." << endl;
							projectile_manager->addProjectile(moveForward(player->getPosition(), yaw, pitch, 1.0), PROJ_SPEED, (yaw*DEG_TO_RADIAN), pitch); //!++!
																																				  //cout << pitch << "\n";
																																				  //		Projectile* bullet = new Projectile(bt_manager, glm::vec3(0, 0, 0), 1);
						}
					}
					if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
				}

				if (sdlEvent.type == SDL_MOUSEBUTTONUP  && pointOfView == FIRST_PERSON) {
					leftClick = false;
					rightClick = false;
				}

				if (player->getState() == ON_GROUND)
					increase = 1.0f;
				else increase = 0.3f;
				if (keys[SDL_SCANCODE_W]) {
					playerBody->setLinearVelocity(speedForward(increase, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED)));
				}
				else if (keys[SDL_SCANCODE_S]) {
					playerBody->setLinearVelocity(speedForward(-increase, yaw, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED)));
				}
				if (keys[SDL_SCANCODE_A]) {
					//player->setPosition(moveRight(player->getPosition(), yaw, -0.1f));
					playerBody->setLinearVelocity(speedRight(-increase, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED)));
				}
				else if (keys[SDL_SCANCODE_D]) {
					//player->setPosition(moveRight(player->getPosition(), yaw, 0.1f));
					playerBody->setLinearVelocity(speedRight(increase, yaw, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED)));
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

				std::pair<std::string, btRigidBody*> selectedObject = std::make_pair(bodies.begin()->first, bodies.begin()->second);

				btVector3 lastObject = selectedObject.second->getWorldTransform().getOrigin();
				btVector3 playerPos = playerBody->getWorldTransform().getOrigin();

				btTransform t;
				t.setIdentity();

				if (sdlEvent.type == SDL_MOUSEBUTTONDOWN && pointOfView == FIRST_PERSON) {
					if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
						if (coolDown <= 0.0f) {
							leftClick = true;
							coolDown = COOL_TIME;
							temp[0] = std::make_tuple(std::get<0>(temp[0]), at, std::get<1>(temp[0]), glm::vec3(0.0f, yaw, 0.0f));
							temp[1] = std::make_tuple(std::get<0>(temp[1]), std::get<1>(temp[0]), std::get<2>(temp[1]), glm::vec3(0.0f, yaw, 0.0f));
							creation = true;
						}
					}
					if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
				}

				if (sdlEvent.type == SDL_MOUSEBUTTONUP  && pointOfView == FIRST_PERSON) {
					leftClick = false;
					rightClick = false;
				}

				int i = 0;
				float scaling = 0;
				if (stage == MODEL) {
					i = 0;
					scaling = MODEL_SCALING;
				}
				else if (stage == BOUNDING) {
					i = 1;
					scaling = BOUNDING_SCALING;
				}

				if (keys[SDL_SCANCODE_W]) {
					glm::vec3 move = moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, 0.1f);
					t.setOrigin(btVector3(move.x, move.y, move.z));
					playerBody->setWorldTransform(t);
				}
				else if (keys[SDL_SCANCODE_S]) {
					glm::vec3 move = moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, -0.1f);
					t.setOrigin(btVector3(move.x, move.y, move.z));
					playerBody->setWorldTransform(t);
				}
				if (keys[SDL_SCANCODE_A]) {
					glm::vec3 move = moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, -0.1f);
					t.setOrigin(btVector3(move.x, move.y, move.z));
					playerBody->setWorldTransform(t);
				}
				else if (keys[SDL_SCANCODE_D]) {
					glm::vec3 move = moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), yaw, 0.1f);
					t.setOrigin(btVector3(move.x, move.y, move.z));
					playerBody->setWorldTransform(t);
				}
				if (keys[SDL_SCANCODE_LSHIFT]) {
					shiftPressed = true;
					std::cout << "Shift pressed. (Increase)" << std::endl;
				}
				if (keys[SDL_SCANCODE_LCTRL]) {
					shiftPressed = false;
					std::cout << "Ctrl pressed. (decrease)" << std::endl;
				}

				if (keys[SDL_SCANCODE_O]) {
					writeFile();
					std::cout << "File saved" << std::endl;
				}

				if (keys[SDL_SCANCODE_8]) {
					boundingType = BOX;
					currentBounding = "box";
					std::cout << "Bounding Type: BOX" << std::endl;
				}
				if (keys[SDL_SCANCODE_9]) {
					boundingType = SPHERE;
					currentBounding = "sphere";
					std::cout << "Bounding Type: SPHERE" << std::endl;
				}
				if (keys[SDL_SCANCODE_0]) {
					boundingType = CAPSULE;
					currentBounding = "capsule";
					std::cout << "Bounding Type: CAPSULE" << std::endl;
				}

				/*	if (keys[SDL_SCANCODE_KP_DIVIDE]) {
						std::map<std::string, btRigidBody*>::iterator it;
						it = bodies.end();
						if (it != bodies.begin()) {
							--it;
						}
						selectedObject =  std::make_pair(it->first, bodies.begin()->second);
						std::cout << selectedObject.first << " selected" << std::endl;
					}
					else if (keys[SDL_SCANCODE_KP_MULTIPLY]) {
						std::map<std::string, btRigidBody*>::iterator it;
						it = bodies.begin();
						if (it != bodies.end()) {
							++it;
						}
						selectedObject = std::make_pair(it->first, bodies.begin()->second);
						std::cout << selectedObject.first << " selected" << std::endl;
					}*/

				if (creation == true) {
					if (keys[SDL_SCANCODE_KP_8]) {
						glm::vec3 move = glm::vec3(moveForward(glm::vec3(std::get<1>(temp[i])), yaw, 0.01));
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
					else if (keys[SDL_SCANCODE_KP_5]) {
						glm::vec3 move = glm::vec3(moveForward(glm::vec3(glm::vec3(std::get<1>(temp[i]))), yaw, -0.01));
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
					if (keys[SDL_SCANCODE_KP_4]) {
						glm::vec3 move = glm::vec3(moveRight(glm::vec3(glm::vec3(std::get<1>(temp[i]))), yaw, -0.01));
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
					else if (keys[SDL_SCANCODE_KP_6]) {
						glm::vec3 move = glm::vec3(moveRight(glm::vec3(glm::vec3(std::get<1>(temp[i]))), yaw, 0.01));
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
				}
				else {
					if (keys[SDL_SCANCODE_KP_8]) {
						glm::vec3 moveLeft = glm::vec3(moveForward(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, 0.1));
						t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
						selectedObject.second->setWorldTransform(t);
					}
					else if (keys[SDL_SCANCODE_KP_5]) {
						glm::vec3 moveLeft = glm::vec3(moveForward(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, -0.1));
						t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
						selectedObject.second->setWorldTransform(t);
					}
					if (keys[SDL_SCANCODE_KP_4]) {
						glm::vec3 moveLeft = glm::vec3(moveRight(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, -0.1));
						t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
						selectedObject.second->setWorldTransform(t);
					}
					else if (keys[SDL_SCANCODE_KP_6]) {
						glm::vec3 moveLeft = glm::vec3(moveRight(glm::vec3(lastObject.x(), lastObject.y(), lastObject.z()), yaw, 0.1));
						t.setOrigin(btVector3(moveLeft.x, moveLeft.y, moveLeft.z));
						selectedObject.second->setWorldTransform(t);
					}
				}

				if (keys[SDL_SCANCODE_KP_PERIOD]) {
					if (coolDown <= 0.0f) {
						coolDown = COOL_TIME;
						if (stage == MODEL) {
							stage = BOUNDING;
							std::cout << "Editing stage: BOUNDING" << std::endl;
						}
						else if (stage == BOUNDING) {
							stage = MODEL;
							std::cout << "Editing stage: MODEL" << std::endl;
						}
					}
				}

				if (keys[SDL_SCANCODE_KP_ENTER]) {
					if (coolDown <= 0.0f) {
						insertBounding(std::get<0>(temp[0]), std::get<2>(temp[0]), std::get<2>(temp[1]), std::get<3>(temp[0]), std::get<3>(temp[1]));
						coolDown = COOL_TIME;
						creation = false;
					}
				}

				if (keys[SDL_SCANCODE_KP_PLUS]) {
					if (coolDown <= 0.0f) {
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
						coolDown = COOL_TIME;
					}
				}
				if (keys[SDL_SCANCODE_KP_MINUS]) {
					if (coolDown <= 0.0f) {
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
						coolDown = COOL_TIME;
					}
				}

				if (shiftPressed) {
					if (creation == true) {
						if (keys[SDL_SCANCODE_KP_1]) { //X scale
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x + scaling, std::get<2>(temp[i]).y, std::get<2>(temp[i]).z), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_2]) { //Y scale
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x, std::get<2>(temp[i]).y + scaling, std::get<2>(temp[i]).z), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_3]) { //Z scale
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x, std::get<2>(temp[i]).y, std::get<2>(temp[i]).z + scaling), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_0]) { //ALL scale
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x + scaling, std::get<2>(temp[i]).y + scaling, std::get<2>(temp[i]).z + scaling), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_7]) { //moveUP
							glm::vec3 move = glm::vec3(std::get<1>(temp[i]).x, std::get<1>(temp[i]).y + 0.01, std::get<1>(temp[i]).z);
							temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_9]) { //moveDOWN
							glm::vec3 move = glm::vec3(std::get<1>(temp[i]).x, std::get<1>(temp[i]).y - 0.01, std::get<1>(temp[i]).z);
							temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
						}
					}
					else {
						if (keys[SDL_SCANCODE_KP_1]) {
							get<1>(models[selectedObject.first]).x += scaling;
						}
						if (keys[SDL_SCANCODE_KP_2]) {
							get<1>(models[selectedObject.first]).y += scaling;
						}
						if (keys[SDL_SCANCODE_KP_3]) {
							get<1>(models[selectedObject.first]).z += scaling;
						}
						if (keys[SDL_SCANCODE_KP_0]) {
							get<1>(models[selectedObject.first]).x += scaling;
							get<1>(models[selectedObject.first]).y += scaling;
							get<1>(models[selectedObject.first]).z += scaling;
						}
					}
				}
				if (!shiftPressed) {
					if (creation == true) {
						if (keys[SDL_SCANCODE_KP_1]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x - scaling, std::get<2>(temp[i]).y, std::get<2>(temp[i]).z), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_2]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x, std::get<2>(temp[i]).y - scaling, std::get<2>(temp[i]).z), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_3]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x, std::get<2>(temp[i]).y, std::get<2>(temp[i]).z - scaling), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_0]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), glm::vec3(std::get<2>(temp[i]).x - scaling, std::get<2>(temp[i]).y - scaling, std::get<2>(temp[i]).z - scaling), std::get<3>(temp[i]));
						}
						if (keys[SDL_SCANCODE_KP_7]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), std::get<2>(temp[i]), glm::vec3(std::get<3>(temp[i]).x, std::get<3>(temp[i]).y - 0.1, std::get<3>(temp[i]).z));
						}
						if (keys[SDL_SCANCODE_KP_9]) {
							temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), std::get<2>(temp[i]), glm::vec3(std::get<3>(temp[i]).x, std::get<3>(temp[i]).y + 0.1, std::get<3>(temp[i]).z));
						}
					}
					else {
						if (keys[SDL_SCANCODE_KP_1]) {
							get<1>(models[selectedObject.first]).x -= scaling;
						}
						if (keys[SDL_SCANCODE_KP_2]) {
							get<1>(models[selectedObject.first]).y -= scaling;
						}
						if (keys[SDL_SCANCODE_KP_3]) {
							get<1>(models[selectedObject.first]).z -= scaling;
						}
						if (keys[SDL_SCANCODE_KP_0]) {
							get<1>(models[selectedObject.first]).x -= scaling;
							get<1>(models[selectedObject.first]).y -= scaling;
							get<1>(models[selectedObject.first]).z -= scaling;
						}
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

	void renderObject(glm::mat4 proj, Model *modelData, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation, GLuint shader, GLuint texture) {
		glm::mat4 model;
		model = glm::translate(model, pos);
		//model = glm::rotate(model, float(-yaw*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, float(rotation.y * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
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
		glm::vec3 gunPos = moveForward(glm::vec3(player->getPosition().x, player->getPosition().y - 0.15, player->getPosition().z), yaw, 0.2);
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

	double gameTime() {
		currentTime = clock();

		unsigned int dt = currentTime - lastTime;
		double dt_secs = (double)dt / 1000;
		if (dt_secs > 0.017) dt_secs = 0.017; // first value is off ( 5.5~)

		lastTime = currentTime;

		return dt_secs;
	}

	void update(SDL_Window * window, SDL_Event sdlEvent) {
		controls(window, sdlEvent);

		dt_secs = gameTime();
		//cout << dt_secs;
		coolDown -= dt_secs;

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
			btVector3 y = (((btBoxShape*)bodies[id_pair.first]->getCollisionShape())->getHalfExtentsWithMargin());
			glm::vec3 spherePosition = glm::vec3(p.x(), p.y(), p.z());
			glm::vec3 position = glm::vec3(p.x(), p.y() - y.y(), p.z());
			glm::vec3 rotation = glm::vec3(bodies[id_pair.first]->getWorldTransform().getRotation().getX(), bodies[id_pair.first]->getWorldTransform().getRotation().getY(), bodies[id_pair.first]->getWorldTransform().getRotation().getZ());
			//	btQuaternion test = bodies[id_pair.first]->getWorldTransform().getRotation();
		//		test.getAngle();

			if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				bt_manager->renderBox(bodies[id_pair.first], view, projection, modelTypes["cube"], shader, groundTexture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				renderObject(projection, modelTypes[get<0>(models[id_pair.first])], position, get<1>(models[id_pair.first]), rotation, shader, groundTexture);
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				bt_manager->renderSphere(bodies[id_pair.first], view, projection, modelTypes["sphere"], shader, defaultTexture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				renderObject(projection, modelTypes[get<0>(models[id_pair.first])], spherePosition, get<1>(models[id_pair.first]), rotation, shader, defaultTexture);
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				bt_manager->renderCapsule(bodies[id_pair.first], view, projection, modelTypes["capsule"], shader, defaultTexture);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				renderObject(projection, modelTypes[get<0>(models[id_pair.first])], spherePosition, get<1>(models[id_pair.first]), rotation, shader, defaultTexture);
			}
			i++;
		}
		///+++++++++++++++
		// RENDERING modelTypes

		if (pointOfView == THIRD_PERSON)
			renderObject(projection, modelTypes["nanosuit"], glm::vec3(player->getPosition().x, player->getPosition().y - 1.75, player->getPosition().z), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.0, -yaw, 0.0), shader, 0);
		// rip robot
		  //		renderObject(projection, modelTypes["robot"], glm::vec3(4.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2), shader);

		if (pointOfView == FIRST_PERSON)
			renderWeapon(projection, modelTypes["plasmacutter"], shader);

		if (mode == EDIT) {
			if (creation == true) {
				renderObject(projection, modelTypes[std::get<0>(temp[0])], std::get<1>(temp[0]), std::get<2>(temp[0]), std::get<3>(temp[0]), shader, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				renderObject(projection, modelTypes[std::get<0>(temp[1])], std::get<1>(temp[1]), std::get<2>(temp[1]), std::get<3>(temp[1]), shader, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDisable(GL_CULL_FACE);
			}
		}

		//!++!
		projectile_manager->renderProjectiles(view, projection, modelTypes["sphere"], shader, defaultTexture, dt_secs);
	}

	// main render function, sets up the shaders and then calls all other functions
	void renderShadowedScene(glm::mat4 projection, glm::mat4 viewMatrix, GLuint shader, bool cubemap) {

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

				renderShadowedScene(projection, view, depthShaderProgram, true); // render using light's point of view and simpler shader program

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			else {
				//Render to frame buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window
																	// clear the screen
				glEnable(GL_CULL_FACE);
				glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

				skybox->renderSkybox(projection, view, modelTypes["cube"]);
				// normal rendering
				renderShadowedScene(projection, view, modelProgram, false); // render normal scene from normal point of view
				// fps counter; 5 of 5
				//h_manager->renderToHud(5, texturedProgram, modelTypes["cube"], glm::vec3(-0.0f, 0.0f, 0.9f));
				if (mode == EDIT) {
					h_manager->renderEditHud("Bounding", currentBounding, texturedProgram, modelTypes["cube"], glm::vec3(0.55f, 0.45f, 0.9f));
					h_manager->renderEditHud("Model", currentModel, texturedProgram, modelTypes["cube"], glm::vec3(0.6f, 0.35f, 0.9f));
					if (coolDown <= 0) {
						h_manager->renderEditHud("Timer", "READY", texturedProgram, modelTypes["cube"], glm::vec3(0.6f, 0.25f, 0.9f));
					}
					else
						h_manager->renderEditHud("Timer", "WAIT", texturedProgram, modelTypes["cube"], glm::vec3(0.6f, 0.25f, 0.9f));
				}
			}
			glDepthMask(GL_TRUE);
		}
		SDL_GL_SwapWindow(window); // swap buffers
	}
}
