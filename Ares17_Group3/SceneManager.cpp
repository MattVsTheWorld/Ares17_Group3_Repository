#include "SceneManager.h"

using namespace std;
typedef std::pair<string, btRigidBody*> bodyID;

// this class still needs a lot of work
namespace SceneManager {

	std::shared_ptr<GlobalData> globalData;
	// Shaders
	GLuint texturedProgram;
	GLuint modelProgram;
	Skybox *skybox;
	Skybox *skybox_2;
	gameState currentState = MENU;
	float pauseTimeout = 1.0f;
	bool clickable = true;
	unsigned int lastTime = 0, currentTime;
	float dt_secs;
	float theta = 0;
	modes mode = PLAY;
	bound boundingType = BOX;
	level currentLevel = FIRST;
	editStages stage = MODEL;
	// AI
	Grid* level1Grid;

	// SHADOWS
	GLuint depthShaderProgram; //shader to create shadow cubemaps
	GLuint depthMapFBO; // FBO
	GLuint depthCubemap;
	const GLuint SHADOW_WIDTH = SCREENWIDTH, SHADOW_HEIGHT = SCREENWIDTH;
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	// near and far act the same as near and far plane for camera, but this is for shadows
	// If the level gets too big and you can see the edge of the shadowmapping, increase the far 
	// (or consider adding multiple lights / opting for a directional light shadowmap)
	GLfloat near_plane = 0.01f;
	GLfloat far_plane = 80.0f;
	//////////////////
	unordered_set<AbstractNPC *> enemies;

	const char *testTexFiles[6] = {
		"../Ares17_Group3/Town-skybox/Town_bk.bmp", "../Ares17_Group3/Town-skybox/Town_ft.bmp", "../Ares17_Group3/Town-skybox/Town_rt.bmp", "../Ares17_Group3/Town-skybox/Town_lf.bmp", "../Ares17_Group3/Town-skybox/Town_up.bmp", "../Ares17_Group3/Town-skybox/Town_dn.bmp"
	};

	const char *skyTexFiles[6] = {
		"../Ares17_Group3/star-skybox/sky_back.bmp", "../Ares17_Group3/star-skybox/sky_front.bmp", "../Ares17_Group3/star-skybox/sky_right.bmp", "../Ares17_Group3/star-skybox/sky_left.bmp", "../Ares17_Group3/star-skybox/sky_top.bmp", "../Ares17_Group3/star-skybox/sky_bot.bmp"
	};

	const char *skyTexFiles_2[6] = {
		"../Ares17_Group3/Violent/violentdays_bk.bmp", "../Ares17_Group3/Violent/violentdays_ft.bmp", "../Ares17_Group3/Violent/violentdays_rt.bmp", "../Ares17_Group3/Violent/violentdays_lf.bmp", "../Ares17_Group3/Violent/violentdays_up.bmp", "../Ares17_Group3/Violent/violentdays_dn.bmp"
	};

	// Load modelTypes
	unordered_map<string, Model*> modelTypes;

	unordered_map<std::string, std::tuple<string, glm::vec3, glm::vec3, glm::vec3>> models; //objType, <modelName, scale, rotation, offset>
	unordered_map<std::string, btRigidBody*> bodies;
	vector<tuple <btPairCachingGhostObject*, string, string>> collectables;

	tuple<std::string, glm::vec3, glm::vec3, glm::vec3> temp[2]; //name, position, scale, rotation

	string currentModel = "box";
	string currentBounding = "box";

	GLuint defaultTexture;
	GLuint groundTexture;
	GLuint groundTexture_2;
	GLuint groundTexture_3;
	GLuint heartTexture;

	glm::mat4 view;

	glm::vec3 rotationAngles(static_cast<GLfloat>(0.0f), static_cast<GLfloat>(0.0f), static_cast<GLfloat>(0.0f));

	glm::vec3 eye(2.0f, 3.0f, -6.0f);
	glm::vec3 at(0.0f, 0.5f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	float mass = 0;
	unsigned int objectID = 0;
	unsigned int worldObjectID = 0;
	std::string selectedKey = "";
	std::string selectedPrev = "";

	bool leftClick = false;
	bool rightClick = false;
	bool creation = false;
	float coolDown = 0.0f; //wait between shots
	bool shiftPressed = true;

	float defeatTime = 3.0f;

	// Old movement methods, used in edit mode (?)
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(rotationAngles.y*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(rotationAngles.y*DEG_TO_RADIAN));
	}

	glm::vec3 moveForward(glm::vec3 pos, glm::vec3 rotationAngles, GLfloat d) { // TODO remove?
		return glm::vec3(pos.x + d*std::sin(rotationAngles.y*DEG_TO_RADIAN), pos.y - d*std::sin(rotationAngles.x), pos.z - d*std::cos(rotationAngles.y*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(rotationAngles.y*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(rotationAngles.y*DEG_TO_RADIAN));
	}
	
	glm::vec3 shiftForward(glm::vec3 pos, glm::vec3 angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(angle.y*DEG_TO_RADIAN), pos.y - d*std::sin(angle.x), pos.z - d*std::cos(angle.y*DEG_TO_RADIAN));
	}

	glm::vec3 shiftRight(glm::vec3 pos, glm::vec3 angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(angle.y*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle.y*DEG_TO_RADIAN));
	}

	static btVector3 getLinearVelocityInBodyFrame(btRigidBody* body)
	{
		return(body->getWorldTransform().getBasis().transpose() *
			body->getLinearVelocity());
	}

	// Speedforward/right allow moving the player in camera direction
	// Also remembered as that one time I managed to fit a conditional operator somewhere
	btVector3 speedForward(GLfloat _speed, GLfloat angle, bool concurrent) {
		btVector3 speed = getLinearVelocityInBodyFrame(globalData->player->playerBody);

		if (!concurrent && speed.absolute().x() <= globalData->player->getSpeed() && speed.absolute().z() <= globalData->player->getSpeed())
			speed = btVector3(speed.x() + _speed*std::sin(angle*DEG_TO_RADIAN), speed.y(), speed.z() - _speed*std::cos(angle*DEG_TO_RADIAN));
		else if (concurrent)
			speed = btVector3(speed.x() + (speed.absolute().x() >globalData->player->getSpeed() ? 0 : _speed*std::sin(angle*DEG_TO_RADIAN)), speed.y(), speed.z() - (speed.absolute().z() > globalData->player->getSpeed() ? 0 : _speed*std::cos(angle*DEG_TO_RADIAN)));
		return speed;
	}

	btVector3 speedRight(GLfloat _speed, GLfloat angle, bool concurrent) {
		btVector3 speed = getLinearVelocityInBodyFrame(globalData->player->playerBody);
		if (!concurrent && speed.absolute().x() <= globalData->player->getSpeed() && speed.absolute().z() <= globalData->player->getSpeed())
			speed = btVector3(speed.x() + _speed*std::cos(angle*DEG_TO_RADIAN), speed.y(), speed.z() + _speed*std::sin(angle*DEG_TO_RADIAN));
		else if (concurrent)
			speed = btVector3(speed.x() + (speed.absolute().x() > globalData->player->getSpeed() ? 0 : _speed*std::cos(angle*DEG_TO_RADIAN)), speed.y(), speed.z() + (speed.absolute().z() > globalData->player->getSpeed() ? 0 : _speed*std::sin(angle*DEG_TO_RADIAN)));
		return speed;
	}

	btVector3 jump(GLfloat _speed) {
		btVector3 speed = getLinearVelocityInBodyFrame(globalData->player->playerBody);
		if (globalData->player->getState() != JUMPING) {
			speed = btVector3(speed.x(), speed.y() + _speed, speed.z());
			globalData->player->setState(JUMPING);
		}
		return speed;
	}

	static void toEulerianAngle(const btQuaternion& q, glm::vec3& rotation)
	{
		double test = q.x()*q.y() + q.z()*q.w();
		if (test > 0.499) { // singularity at north pole
			rotation.y = 2 * atan2(q.x(), q.w());
			rotation.x = PI / 2.0f;
			rotation.z = 0;
			return;
		}
		if (test < -0.499) { // singularity at south pole
			rotation.y = -2 * atan2(q.x(), q.w());
			rotation.x = -PI / 2.0f;
			rotation.z = 0;
			return;
		}
		double sqx = q.x()*q.x();
		double sqy = q.y()*q.y();
		double sqz = q.z()*q.z();
		rotation.y = atan2(2 * q.y()*q.w() - 2 * q.x()*q.z(), 1 - 2 * sqy - 2 * sqz);
		rotation.x = asin(2 * test);
		rotation.z = atan2(2 * q.x()*q.w() - 2 * q.y()*q.z(), 1 - 2 * sqx - 2 * sqz);
	}

	int boxNo = 0;
	int sphereNo = 0;
	int capsuleNo = 0;

	void writeFile() {
		// writing on a text file
		ofstream myfile("../Ares17_Group3/bodies.txt");
		if (myfile.is_open())
		{
			myfile << "BoundingName | ModelName | Positions | BoundingScale | ModelScale | Model Rotation | Bounding Rotation | Offset | Mass \t//numberOfBodies on first line\n";
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
				glm::vec3 offset;
				float mass;
				//BoundingType | ModelName | Positions | BoundingScale, ModelScale, mASs
				if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
					objType = "box";
					objType.append(std::to_string(boxNo));
					boundingType = bodies.find(objType)->first;
					modelName = get<0>(models.find(objType)->second);
					position = bodies.find(objType)->second->getWorldTransform().getOrigin();
					boundingScale = (((btBoxShape*)bodies.find(objType)->second->getCollisionShape())->getHalfExtentsWithMargin()); //*2 as its half
					boundingRotation = glm::vec3(bodies.find(objType)->second->getWorldTransform().getRotation().getX(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getY(),
						bodies.find(objType)->second->getWorldTransform().getRotation().getZ());
					modelScale = get<1>(models.find(objType)->second);
					modelRotation = get<2>(models.find(objType)->second);
					offset = get<3>(models.find(objType)->second);
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
					myfile << offset.x << ",";
					myfile << offset.y << ",";
					myfile << offset.z << ",";
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
					offset = get<3>(models.find(objType)->second);
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
					myfile << offset.x << ",";
					myfile << offset.y << ",";
					myfile << offset.z << ",";
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
		ifstream myfile;
		if (currentLevel == FIRST)
			myfile = ifstream("../Ares17_Group3/level1.txt");
		else if (currentLevel == SECOND)
			myfile = ifstream("../Ares17_Group3/level2.txt");
		else if (currentLevel == BOSS)
			myfile = ifstream("../Ares17_Group3/levelBoss.txt");
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
				glm::vec3 offset;
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
								variables = 20;
								float digit = stof(asString);
								switch (i) {
								case 2: position.x = digit;				break;
								case 3: position.y = digit;				break;
								case 4: position.z = digit;				break;
								case 5: boundingScale.x = digit;		break;
								case 6:	boundingScale.y = digit;		break;
								case 7: boundingScale.z = digit;		break;
								case 8: modelScale.x = digit;			break;
								case 9: modelScale.y = digit;			break;
								case 10: modelScale.z = digit;			break;
								case 11: modelRotation.x = digit;		break;
								case 12: modelRotation.y = digit;		break;
								case 13: modelRotation.z = digit;		break;
								case 14: boundingRotation.x = digit;	break;
								case 15: boundingRotation.y = digit;	break;
								case 16: boundingRotation.z = digit;	break;
								case 17: offset.x = digit;				break;
								case 18: offset.y = digit;				break;
								case 19: offset.z = digit;				break;
								case 20: mass = digit;					break;
								}
							}
							else if (key.find("sphere") != std::string::npos) {
								variables = 9;
								float digit = stof(asString);
								switch (i) {
								case 2:	position.x = digit;				break;
								case 3:	position.y = digit;				break;
								case 4:	position.z = digit;				break;
								case 5:	radius = digit;					break;
								case 6:	modelScale.x = digit;			break;
								case 7:	modelScale.y = digit;			break;
								case 8:	modelScale.z = digit;			break;
								case 9:	mass = digit;					break;
								}
							}
							else if (key.find("capsule") != std::string::npos) {
								variables = 19;
								float digit = stof(asString);
								switch (i) {
								case 2:	position.x = digit;				break;
								case 3:	position.y = digit;				break;
								case 4:	position.z = digit;				break;
								case 5:	radius = digit;					break;
								case 6:	height = digit;					break;
								case 7:	modelScale.x = digit;			break;
								case 8:	modelScale.y = digit;			break;
								case 9:	modelScale.z = digit;			break;
								case 10: modelRotation.x = digit;		break;
								case 11: modelRotation.y = digit;		break;
								case 12: modelRotation.z = digit;		break;
								case 13: boundingRotation.x = digit;	break;
								case 14: boundingRotation.y = digit;	break;
								case 15: boundingRotation.z = digit;	break;
								case 16: offset.x = digit;				break;
								case 17: offset.y = digit;				break;
								case 18: offset.z = digit;				break;
								case 19: mass = digit;					break;
								}
							}
						}//else
					}//for loop
					bodyNo++;
				}
				if (key.find("box") != std::string::npos) {
					if (modelName == "heart" || modelName == "shield") {
						btRigidBody *temp = globalData->bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, position.x, position.y, position.z, mass, COL_COLLECTABLE, COL_PLAYER);

						bodies.insert(std::pair<string, btRigidBody*>(key, temp));
						btPairCachingGhostObject* tempGhost = new btPairCachingGhostObject();
						tempGhost->setCollisionShape(temp->getCollisionShape());
						tempGhost->setWorldTransform(temp->getWorldTransform());
						tempGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
						collectables.push_back(make_tuple(tempGhost, key, modelName));
						globalData->bt_manager->addGhostToWorld(tempGhost, COL_COLLECTABLE, COL_PLAYER);
					}
					else
						bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, modelRotation, offset)));
				}
				else if (key.find("sphere") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addSphere(radius, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, glm::vec3(0., 0., 0.), offset)));
				}
				else if (key.find("capsule") != std::string::npos) {
					bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addCapsule(radius, height, position.x, position.y, position.z, mass)));
					models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3, glm::vec3>>(key, std::make_tuple(modelName, modelScale, modelRotation, offset)));
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
	}

	void initmodelTypes() {
		//Enemies
		modelTypes.insert(std::pair<string, Model*>("enforcerAttack", new Model("../Ares17_Group3/Models/Enemies/Enforcer/Attack/standing_melee_attack_downward.dae")));
		modelTypes.insert(std::pair<string, Model*>("enforcerRun", new Model("../Ares17_Group3/Models/Enemies/Enforcer/Run/running.dae")));
		modelTypes.insert(std::pair<string, Model*>("enforcerDie", new Model("../Ares17_Group3/Models/Enemies/Enforcer/Die/falling_back_death.dae")));

		modelTypes.insert(std::pair<string, Model*>("assaultAttack", new Model("../Ares17_Group3/Models/Enemies/Assault/Attack/gunplay.dae")));
		modelTypes.insert(std::pair<string, Model*>("assaultRun", new Model("../Ares17_Group3/Models/Enemies/Assault/Run/run_with_sword.dae")));
		modelTypes.insert(std::pair<string, Model*>("assaultDie", new Model("../Ares17_Group3/Models/Enemies/Assault/Die/falling_back_death.dae")));

		modelTypes.insert(std::pair<string, Model*>("swarmerAttack", new Model("../Ares17_Group3/Models/Enemies/Swarmer/Attack/Standing_Torch_Melee_Attack_01.dae")));
		modelTypes.insert(std::pair<string, Model*>("swarmerRun", new Model("../Ares17_Group3/Models/Enemies/Swarmer/Run/run.dae")));
		modelTypes.insert(std::pair<string, Model*>("swarmerDie", new Model("../Ares17_Group3/Models/Enemies/Swarmer/Die/flying_back_death.dae")));

		modelTypes.insert(std::pair<string, Model*>("bossAttack", new Model("../Ares17_Group3/Models/Enemies/Boss/Attack/mutant_swiping.dae")));
		modelTypes.insert(std::pair<string, Model*>("bossRun", new Model("../Ares17_Group3/Models/Enemies/Boss/Run/mutant_run.dae")));
		modelTypes.insert(std::pair<string, Model*>("bossDie", new Model("../Ares17_Group3/Models/Enemies/Boss/Die/sword_and_shield_death.dae")));
		
		//Environment
		modelTypes.insert(std::pair<string, Model*>("sphere", new Model("../Ares17_Group3/Models/Environment/sphere.obj")));
		modelTypes.insert(std::pair<string, Model*>("capsule", modelTypes["sphere"]));
		modelTypes.insert(std::pair<string, Model*>("house", new Model("../Ares17_Group3/Models/Environment/House/houselow.obj")));
		modelTypes.insert(std::pair<string, Model*>("carpile", new Model("../Ares17_Group3/Models/Environment/CarPile/wasteddisplay.obj")));
		modelTypes.insert(std::pair<string, Model*>("oiltank", new Model("../Ares17_Group3/Models/Environment/OilTank/Oiltank.obj")));
		modelTypes.insert(std::pair<string, Model*>("catjeep", new Model("../Ares17_Group3/Models/Environment/cyberpunk-truck/hovertruck_lowpoly.obj")));
		modelTypes.insert(std::pair<string, Model*>("heli", new Model("../Ares17_Group3/Models/Environment/Helicopter/hheli.obj")));
		modelTypes.insert(std::pair<string, Model*>("tower", new Model("../Ares17_Group3/Models/Environment/sci-fi-tower/building_02_fbx.FBX.obj")));
		modelTypes.insert(std::pair<string, Model*>("barrier", new Model("../Ares17_Group3/Models/Environment/Barrier/model.obj")));
		
		//Collectable
		modelTypes.insert(std::pair<string, Model*>("heart", new Model("../Ares17_Group3/Models/Collectable/Heart/Heart.obj")));
		modelTypes.insert(std::pair<string, Model*>("shield", new Model("../Ares17_Group3/Models/Collectable/Shield/shield.obj")));

		//Guns
		modelTypes.insert(std::pair<string, Model*>("scifigun", new Model("../Ares17_Group3/Models/Guns/Scifi/25ad7fc3a09f4a958dd62b5b522257ee.obj")));
		modelTypes.insert(std::pair<string, Model*>("scifipistol", new Model("../Ares17_Group3/Models/Guns/ScifiPistol/ceeb75e9f4e34b6191d92c38a470453d.obj")));
		modelTypes.insert(std::pair<string, Model*>("nukacola", new Model("../Ares17_Group3/Models/Guns/NukaCola/NukaColaGun.obj")));
	}

	void insertBounding(glm::vec3 boundingPos, glm::vec3 modelScale, glm::vec3 boundingScale, glm::vec3 modelRotation, glm::vec3 boundingRotation, float mass) {

		std::string key;

		if (boundingType == BOX) {
			key = "box";
			key.append(to_string(boxNo));
			bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addBox(boundingScale.x, boundingScale.y, boundingScale.z, boundingPos.x, boundingPos.y, boundingPos.z, mass)));
			cout << key << " Added\n";
			boxNo++;
		}
		if (boundingType == SPHERE) {
			key = "sphere";
			key.append(to_string(sphereNo));
			bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addSphere(boundingScale.x, boundingPos.x, boundingPos.y, boundingPos.z, mass)));
			cout << key << "Added\n";
			sphereNo++;
		}
		if (boundingType == CAPSULE) {
			key = "capsule";
			key.append(to_string(capsuleNo));
			//boundingScale.x = radius, .y = height
			bodies.insert(std::pair<string, btRigidBody*>(key, globalData->bt_manager->addCapsule(boundingScale.x, boundingScale.y, boundingPos.x, boundingPos.y, boundingPos.z, mass)));
			cout << key << "Added\n";
			capsuleNo++;
		}
		if (boundingType != SPHERE)
		{
			bodies[key]->setAngularFactor(0); // Doesn't fall sideways
		}
		bodies[key]->setFriction(8);
		models.insert(std::pair<string, std::tuple<string, glm::vec3, glm::vec3, glm::vec3>>(key, make_tuple(currentModel, modelScale, modelRotation, glm::vec3(0.0f, 0.0f, 0.0f))));
	}

	bool findCollision(btPairCachingGhostObject* ghostObject) { // ignore player?
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray =
			ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; ++i) {
			manifoldArray.clear();
			const btBroadphasePair& pair = pairArray[i];
			btBroadphasePair* collisionPair = globalData->bt_manager->findWorldPair(pair);
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
						return true;
					}
				}
			}
		}
		return false;
	}

	void initEnemies() {
		if (currentLevel != BOSS) {
			enemies.insert(new Ranged(new NonPC(125, 6,
				globalData->bt_manager, glm::vec3(0, 5, 0),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

			enemies.insert(new Ranged(new NonPC(125, 6,
				globalData->bt_manager, glm::vec3(25, 5, 0),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

			enemies.insert(new Melee(new NonPC(200, 3,
				globalData->bt_manager, glm::vec3(0, 5, 0),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

			enemies.insert(new Melee(new NonPC(200, 3,
				globalData->bt_manager, glm::vec3(25, 5, 5),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

			enemies.insert(new Light(new NonPC(80, 3,
				globalData->bt_manager, glm::vec3(0, 5, -10),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

			enemies.insert(new Light(new NonPC(80, 3,
				globalData->bt_manager, glm::vec3(0, 5, -25),
				1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));
		}
		else
			enemies.insert(new Heavy(new NonPC(300, 5,
				globalData->bt_manager, glm::vec3(0, 5, 0),
				1.25, 2.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));
	}

	void reset() {
		currentState = MENU;
		globalData->player->restart();
		defeatTime = 3.0f;

		for (auto it = enemies.begin(); it != enemies.end(); )
			it = enemies.erase(it);
		initEnemies();
		for (const auto it : bodies)
			globalData->bt_manager->removeObject(it.second);
		for (const auto it : collectables)
			globalData->bt_manager->removeObject(get<0>(it));

		collectables.clear();
		bodies.clear();
		models.clear();
		initBoxes();
	}
	static const GLuint MAX_BONES = 50;
	GLuint m_boneLocation[MAX_BONES];

	void init(SDL_Window * window) {

		globalData = make_shared<GlobalData>(eye);

		// Preliminary loading for loading screen
		texturedProgram = ShaderManager::initShaders("../Ares17_Group3/Shaders/textured.vert", "../Ares17_Group3/Shaders/textured.frag");
		modelTypes.insert(std::pair<string, Model*>("cube", new Model("../Ares17_Group3/Models/Environment/cube.obj")));
		modelTypes.insert(std::pair<string, Model*>("box", modelTypes["cube"]));
		globalData->h_manager->renderLoading(texturedProgram, modelTypes["cube"]);
		SDL_GL_SwapWindow(window); // swap buffers once
		
		level1Grid = new Grid();
		initmodelTypes();
		modelProgram = ShaderManager::initShaders("../Ares17_Group3/Shaders/modelLoading.vert", "../Ares17_Group3/Shaders/modelLoading.frag");
		//+++
		depthShaderProgram = ShaderManager::initShaders("../Ares17_Group3/Shaders/simpleShadowMap.vert", "../Ares17_Group3/Shaders/simpleShadowMap.frag", "../Ares17_Group3/Shaders/simpleShadowMap.gs");

		for (unsigned int i = 0; i < MAX_BONES; i++) {
			char Name[MAX_BONES];
			snprintf(Name, MAX_BONES, "gBones[%d]", i);
			m_boneLocation[i] = glGetUniformLocation(modelProgram, Name);
		}

		defaultTexture = loadBitmap::loadBitmap("../Ares17_Group3/Textures/wall.bmp");
		groundTexture = loadBitmap::loadBitmap("../Ares17_Group3/Textures/terrain.bmp");
		groundTexture_2 = loadBitmap::loadBitmap("../Ares17_Group3/Textures/terrain_2.bmp");
		groundTexture_3 = loadBitmap::loadBitmap("../Ares17_Group3/Textures/boss.bmp");
		heartTexture = loadBitmap::loadBitmap("../Ares17_Group3/Textures/ruby.bmp");

		initBoxes();
		temp[0] = std::make_tuple(currentModel, at, glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.0f, rotationAngles.y, 0.0f));
		temp[1] = std::make_tuple(currentBounding, std::get<1>(temp[0]), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.0f, rotationAngles.y, 0.0f));


		skybox = new Skybox(skyTexFiles);
		skybox_2 = new Skybox(skyTexFiles_2);

		glm::mat4 projection;

		initEnemies();

		globalData->sound_manager->playSound(globalData->sound_manager->getSound(BG), 0.2f, 2);
		if (!globalData->sound_manager->getState())
			BASS_Pause();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		////////////////////
		/// FBO for shadows
		/////////////////////
		glGenFramebuffers(1, &depthMapFBO);
		// Create depth cubemap texture
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

	void lockCamera()
	{
		if (rotationAngles.x > PITCHLOCK)
			rotationAngles.x = PITCHLOCK;
		if (rotationAngles.x < -PITCHLOCK)
			rotationAngles.x = -PITCHLOCK;
		if (rotationAngles.y < 0.0)
			rotationAngles.y += 360.0;
		if (rotationAngles.y > 360.0)
			rotationAngles.y -= 360;

	}

	float gameTime() {
		currentTime = clock();

		unsigned int dt = currentTime - lastTime;
		float dt_secs = (float)dt / 1000;
		if (dt_secs > 0.017f) dt_secs = 0.017f; // first value is off ( 5.5~)

	
		lastTime = currentTime;

		return dt_secs;
	}

	float AttackingTime = 0.0f;
	float RunningTime = 0.0f;
	float DyingTime = 0.0f;

	//*****************************CONTROLS********************
	void controls(SDL_Window * window, SDL_Event sdlEvent) {
		int MidX = SCREENWIDTH / 2;
		int MidY = SCREENHEIGHT / 2;

		if (currentState == RUNNING) {
			SDL_ShowCursor(SDL_DISABLE);
			int tmpx, tmpy;
			SDL_GetMouseState(&tmpx, &tmpy);
			rotationAngles.y -= 0.1f*(MidX - tmpx); //for y
			rotationAngles.x -= 0.1f*(MidY - tmpy) / 10.0f; //for x
			lockCamera();

			//rotate the camera (move everything in the opposit direction)
			glRotatef(-rotationAngles.x, 1.0, 0.0, 0.0); //basically glm::rotate
			glRotatef(-rotationAngles.y, 0.0, 1.0, 0.0);

			SDL_WarpMouseInWindow(window, MidX, MidY);
		} else if (currentState == PAUSE || currentState == MENU)
			SDL_ShowCursor(SDL_ENABLE);

		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(0, 10, -10));
		btMotionState* motion = new btDefaultMotionState(t);
		motion->setWorldTransform(t);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_P] && clickable) {
			clickable = false;
			// Pausee
			if (currentState == RUNNING) {
				for (const auto it : enemies) 
					it->setState(PAUSED);
				currentState = PAUSE;
				globalData->sound_manager->setState(false);
				BASS_Pause();
			}
			// Unpause
			else if (currentState == PAUSE) {
				for (const auto it : enemies)
					it->setState(TRIGGERED);
				currentState = RUNNING;
				globalData->sound_manager->setState(true);
				BASS_Start();

			}
		}
		
		pauseTimeout -= dt_secs;
		if (pauseTimeout <= 0) {
			pauseTimeout = 1.0f;
			clickable = true;
		}
		
		if (keys[SDL_SCANCODE_LEFTBRACKET]) { mode = PLAY; }
		else if (keys[SDL_SCANCODE_RIGHTBRACKET]) {	mode = EDIT; }

		float increase;
		if (mode == PLAY) {
			if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					if (coolDown <= 0.0f && currentState == RUNNING) {
						leftClick = true;
						if (globalData->player->getWeapon().getName() == PISTOL) {
							globalData->projectile_manager->addProjectile(shiftRight(moveForward(glm::vec3(globalData->player->getPosition().x, globalData->player->getPosition().y - 0.35, globalData->player->getPosition().z),
								rotationAngles, 1.0f), rotationAngles, 0.5), PROJ_SPEED, (rotationAngles.y*DEG_TO_RADIAN), rotationAngles.x); //!++!
							
							coolDown = COOL_TIME;
						globalData->sound_manager->playSound(globalData->sound_manager->getSound(BULLET), 1, 1);
						}

						if (globalData->player->getWeapon().getName() == NUKA) {
							globalData->projectile_manager->addProjectile(shiftRight(moveForward(glm::vec3(globalData->player->getPosition().x, globalData->player->getPosition().y - 0.35, globalData->player->getPosition().z),
								rotationAngles, 1.0f), rotationAngles, 0.5), PROJ_SPEED, (rotationAngles.y*DEG_TO_RADIAN), rotationAngles.x); //!++!

							coolDown = COOL_TIME/2;
						globalData->sound_manager->playSound(globalData->sound_manager->getSound(LASER), 1, 1);																												
						}

						if (globalData->player->getWeapon().getName() == SCIFI) {
							globalData->projectile_manager->addProjectile(shiftRight(moveForward(glm::vec3(globalData->player->getPosition().x, globalData->player->getPosition().y - 0.35, globalData->player->getPosition().z),
								rotationAngles, 1.0f), rotationAngles, 0.5), PROJ_SPEED, (rotationAngles.y*DEG_TO_RADIAN), rotationAngles.x); //!++!

							coolDown = COOL_TIME*2.0;
						globalData->sound_manager->playSound(globalData->sound_manager->getSound(WINCH), 0.4f, 1);																											
						}
					}
				}
				if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
			}

			if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
				leftClick = false;
				rightClick = false;
			}

			if (globalData->player->getState() == ON_GROUND)
				increase = 1.0f;
			else increase = 0.3f;
			if (keys[SDL_SCANCODE_W]) {
				glm::vec3 posBefore = globalData->player->getPosition();
				globalData->player->playerBody->setLinearVelocity(speedForward(increase, rotationAngles.y, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED)));
				glm::vec3 posAfter = globalData->player->getPosition();
			}
			else if (keys[SDL_SCANCODE_S]) {
				globalData->player->playerBody->setLinearVelocity(speedForward(-increase, rotationAngles.y, (keys[SDL_SCANCODE_A] == SDL_PRESSED || keys[SDL_SCANCODE_D] == SDL_PRESSED)));
			}
			if (keys[SDL_SCANCODE_A]) {
				globalData->player->playerBody->setLinearVelocity(speedRight(-increase, rotationAngles.y, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED)));
			}
			else if (keys[SDL_SCANCODE_D]) {
				globalData->player->playerBody->setLinearVelocity(speedRight(increase, rotationAngles.y, (keys[SDL_SCANCODE_W] == SDL_PRESSED || keys[SDL_SCANCODE_S] == SDL_PRESSED)));
			}

			if (keys[SDL_SCANCODE_SPACE]) {
				globalData->player->playerBody->setLinearVelocity(jump(SPEED_CAP_Y));
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

			btVector3 playerPos = globalData->player->playerBody->getWorldTransform().getOrigin();

			btTransform t;
			t.setIdentity();
			
			if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					if (coolDown <= 0.0f) {
						leftClick = true;
						coolDown = COOL_TIME;
						temp[0] = std::make_tuple(currentModel, at, std::get<2>(temp[0]), glm::vec3(0.0f, rotationAngles.y, 0.0f));
						temp[1] = std::make_tuple(currentBounding, std::get<1>(temp[0]), std::get<2>(temp[1]), glm::vec3(0.0f, rotationAngles.y, 0.0f));
						creation = true;
					}
				}
				if (sdlEvent.button.button == SDL_BUTTON_RIGHT) rightClick = true;
			}

			if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
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
				glm::vec3 move = moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), rotationAngles.y, 0.1f);
				t.setOrigin(btVector3(move.x, move.y, move.z));
				globalData->player->playerBody->setWorldTransform(t);
			}
			else if (keys[SDL_SCANCODE_S]) {
				glm::vec3 move = moveForward(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), rotationAngles.y, -0.1f);
				t.setOrigin(btVector3(move.x, move.y, move.z));
				globalData->player->playerBody->setWorldTransform(t);
			}
			if (keys[SDL_SCANCODE_A]) {
				glm::vec3 move = moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), rotationAngles.y, -0.1f);
				t.setOrigin(btVector3(move.x, move.y, move.z));
				globalData->player->playerBody->setWorldTransform(t);
			}
			else if (keys[SDL_SCANCODE_D]) {
				glm::vec3 move = moveRight(glm::vec3(playerPos.x(), playerPos.y(), playerPos.z()), rotationAngles.y, 0.1f);
				t.setOrigin(btVector3(move.x, move.y, move.z));
				globalData->player->playerBody->setWorldTransform(t);
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

			if (creation == true) {
				if (keys[SDL_SCANCODE_KP_8]) {
					glm::vec3 move = glm::vec3(moveForward(glm::vec3(std::get<1>(temp[i])), rotationAngles.y, 0.01f));
					temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
				}
				else if (keys[SDL_SCANCODE_KP_5]) {
					glm::vec3 move = glm::vec3(moveForward(glm::vec3(glm::vec3(std::get<1>(temp[i]))), rotationAngles.y, -0.01f));
					temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
				}
				if (keys[SDL_SCANCODE_KP_4]) {
					glm::vec3 move = glm::vec3(moveRight(glm::vec3(glm::vec3(std::get<1>(temp[i]))), rotationAngles.y, -0.01f));
					temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
				}
				else if (keys[SDL_SCANCODE_KP_6]) {
					glm::vec3 move = glm::vec3(moveRight(glm::vec3(glm::vec3(std::get<1>(temp[i]))), rotationAngles.y, 0.01f));
					temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
				}
				if (shiftPressed) {
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
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
					if (keys[SDL_SCANCODE_KP_9]) { //moveDOWN
						glm::vec3 move = glm::vec3(std::get<1>(temp[i]).x, std::get<1>(temp[i]).y - 0.01, std::get<1>(temp[i]).z);
						temp[i] = make_tuple(std::get<0>(temp[i]), move, std::get<2>(temp[i]), std::get<3>(temp[i]));
					}
				}
				if (!shiftPressed) {
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
					if (keys[SDL_SCANCODE_KP_7]) { //rotate left
						temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), std::get<2>(temp[i]), glm::vec3(std::get<3>(temp[i]).x, std::get<3>(temp[i]).y - 0.1, std::get<3>(temp[i]).z));
					}
					if (keys[SDL_SCANCODE_KP_9]) { //rotate right
						temp[i] = make_tuple(std::get<0>(temp[i]), std::get<1>(temp[i]), std::get<2>(temp[i]), glm::vec3(std::get<3>(temp[i]).x, std::get<3>(temp[i]).y + 0.1, std::get<3>(temp[i]).z));
					}
				}
			}
			else {
				if (keys[SDL_SCANCODE_KP_DIVIDE]) {
					if (coolDown <= 0.0f) {
						if (worldObjectID < 0)
							worldObjectID = 0;
						else
							worldObjectID--;
						int i = 0;
						for (const auto& id_pair : models) {
							if (i == worldObjectID) {
								selectedKey = id_pair.first;
								selectedPrev = get<0>(id_pair.second);
							}
							i++;
						}
						std::cout << "Prev added " << selectedPrev << " selected" << std::endl;
						coolDown = COOL_TIME;
					}
				}
				if (keys[SDL_SCANCODE_KP_MULTIPLY]) {
					if (coolDown <= 0.0f) {
						if (worldObjectID > models.size() - 1)
							worldObjectID = models.size() - 1;
						else
							worldObjectID++;
						int i = 0;
						for (const auto& id_pair : models) {
							if (i == worldObjectID) {
								selectedKey = id_pair.first;
								selectedPrev = get<0>(id_pair.second);
							}
							i++;
						}
						std::cout << "Prev added " << selectedPrev << " selected" << std::endl;
						coolDown = COOL_TIME;
					}
				}

				if (keys[SDL_SCANCODE_KP_7]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x, get<3>(models[selectedKey]).y -= 0.1f, get<3>(models[selectedKey]).z));
				}
				else if (keys[SDL_SCANCODE_KP_9]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x, get<3>(models[selectedKey]).y += 0.1f, get<3>(models[selectedKey]).z));
				}
				if (keys[SDL_SCANCODE_KP_8]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x, get<3>(models[selectedKey]).y, get<3>(models[selectedKey]).z += 0.1f));
				}
				else if (keys[SDL_SCANCODE_KP_5]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x, get<3>(models[selectedKey]).y, get<3>(models[selectedKey]).z -= 0.1f));
				}
				if (keys[SDL_SCANCODE_KP_4]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x -= 0.1f, get<3>(models[selectedKey]).y, get<3>(models[selectedKey]).z));
				}
				else if (keys[SDL_SCANCODE_KP_6]) {
					models[selectedKey] = make_tuple(selectedPrev, get<1>(models[selectedKey]), get<2>(models[selectedKey]), glm::vec3(get<3>(models[selectedKey]).x += 0.1f, get<3>(models[selectedKey]).y, get<3>(models[selectedKey]).z));
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

			if (keys[SDL_SCANCODE_KP_ENTER]) {
				if (coolDown <= 0.0f) {
					insertBounding(std::get<1>(temp[1]), std::get<2>(temp[0]), std::get<2>(temp[1]), std::get<3>(temp[0]), std::get<3>(temp[1]), mass);
					coolDown = COOL_TIME;
					creation = false;
				}
			}

			if (keys[SDL_SCANCODE_EQUALS]) {
				mass += 0.5;
			}
			if (keys[SDL_SCANCODE_MINUS]) {
				if (mass > 0) {
					mass -= 0.5;
				}
			}
		}

		if (keys[SDL_SCANCODE_1]) {
			if (currentState == MENU)
				currentState = PAUSE;
			if (currentState == RUNNING)
				globalData->player->setWeapon(PISTOL);
		}
		if (keys[SDL_SCANCODE_2]) {
			if (currentState == MENU) {
				reset();
				currentState = PAUSE;
			}
			if (currentState == RUNNING)
				globalData->player->setWeapon(NUKA);
		}
		if (keys[SDL_SCANCODE_3]) {
			if (currentState == MENU && clickable == true) {
				globalData->shadows = !globalData->shadows;
				clickable = false;
			}
			if (currentState == RUNNING)
				globalData->player->setWeapon(SCIFI);

		}

		if (keys[SDL_SCANCODE_5]) {
			currentLevel = BOSS;
			globalData->sound_manager->stopBG();
			globalData->sound_manager->playSound(globalData->sound_manager->getSound(BG_2), 0.2f, 2);
		}

		if (keys[SDL_SCANCODE_ESCAPE]) {
			if (currentState == PAUSE)
				currentState = MENU;
		}

		if (keys[SDL_SCANCODE_6])
			exit(0);
	}

	void renderObject(glm::mat4 proj, Model *modelData, glm::vec3 pos, glm::vec3 scale, btQuaternion& rotation, GLuint shader, GLuint texture, bool spin) {

		glm::mat4 model;
		model = glm::translate(model, pos);
		glm::vec3 eulerRotation;
		toEulerianAngle(rotation, eulerRotation);
		model = glm::rotate(model, eulerRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		if (spin)
			model = glm::rotate(model, eulerRotation.y+theta, glm::vec3(0.0f, 1.0f, 0.0f));
		else
			model = glm::rotate(model, eulerRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, eulerRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		
		model = glm::scale(model, scale);	// It's a bit too big for our scene, so scale it down
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(shader, "animated"), 0); //zero is no animations
		modelData->Draw(shader);
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	vector<Matrix4f> assaultAttackTransforms; vector<Matrix4f> assaultRunTransforms; vector<Matrix4f> assaultDieTransforms;
	vector<Matrix4f> enforcerAttackTransforms; vector<Matrix4f> enforcerRunTransforms; vector<Matrix4f> enforcerDieTransforms;
	vector<Matrix4f> swarmerAttackTransforms; vector<Matrix4f> swarmerRunTransforms; vector<Matrix4f> swarmerDieTransforms;
	vector<Matrix4f> bossAttackTransforms; vector<Matrix4f> bossRunTransforms; vector<Matrix4f> bossDieTransforms;

	void SetBoneTransform(npcState state, string name) {
		if (currentLevel == BOSS) {
			if (state == ATTACKING) {
				for (GLuint i = 0; i < bossAttackTransforms.size(); i++) {
					assert(i < MAX_BONES);
					glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& bossAttackTransforms[i]);
				}
			}
			else if (state == DYING) {
				for (GLuint i = 0; i < bossDieTransforms.size(); i++) {
					assert(i < MAX_BONES);
					glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& bossDieTransforms[i]);
				}
			}
			else {
				for (GLuint i = 0; i < bossRunTransforms.size(); i++) {
					assert(i < MAX_BONES);
					glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& bossRunTransforms[i]);
				}
			}
		}
		else {
			if (state == ATTACKING) {
				if (name == "assault") {
					for (GLuint i = 0; i < assaultAttackTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& assaultAttackTransforms[i]);
					}
				}
				else if (name == "enforcer") {
					for (GLuint i = 0; i < enforcerAttackTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& enforcerAttackTransforms[i]);
					}
				}
				else {
					for (GLuint i = 0; i < swarmerAttackTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& swarmerAttackTransforms[i]);
					}
				}
			}
			else if (state == DYING) {
				if (name == "assault") {
					for (GLuint i = 0; i < assaultDieTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& assaultDieTransforms[i]);
					}
				}
				else if (name == "enforcer") {
					for (GLuint i = 0; i < enforcerDieTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& enforcerDieTransforms[i]);
					}
				}
				else {
					for (GLuint i = 0; i < swarmerDieTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& swarmerDieTransforms[i]);
					}
				}
			}
			else {
				if (name == "assault") {
					for (GLuint i = 0; i < assaultRunTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& assaultRunTransforms[i]);
					}
				}
				else if (name == "enforcer") {
					for (GLuint i = 0; i < enforcerRunTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& enforcerRunTransforms[i]);
					}
				}
				else {
					for (GLuint i = 0; i < swarmerRunTransforms.size(); i++) {
						assert(i < MAX_BONES);
						glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)& swarmerRunTransforms[i]);
					}
				}
			}
		}
	}

	int attackingEnemies = 0;
	int runningEnemies = 0;
	int dyingEnemies = 0;
	int animationOn = 0;
		
	void animationTransforms() {
		float speed;
		if (currentLevel == BOSS) { speed = 1; }
		else { speed = 1.5; }
		//only play if animation is being used by something
		if (attackingEnemies > 0)
			animationOn++;
		if (dyingEnemies > 0)
			animationOn++;
		if (runningEnemies > 0)
			animationOn++;

		if (attackingEnemies > 0) {
			AttackingTime += dt_secs;
			if (currentLevel != BOSS) {
				modelTypes["assaultAttack"]->BoneTransform(AttackingTime, assaultAttackTransforms, speed);
				modelTypes["enforcerAttack"]->BoneTransform(AttackingTime, enforcerAttackTransforms, speed);
				modelTypes["swarmerAttack"]->BoneTransform(AttackingTime, swarmerAttackTransforms, speed);
			}
			else
				modelTypes["bossAttack"]->BoneTransform(AttackingTime, bossAttackTransforms, speed);
		}
		else {
			AttackingTime = 0.0f;
		}
		if (dyingEnemies > 0) {
			DyingTime += dt_secs;
			if (currentLevel != BOSS) {
				modelTypes["assaultDie"]->BoneTransform(DyingTime, assaultDieTransforms, speed);
				modelTypes["enforcerDie"]->BoneTransform(DyingTime, enforcerDieTransforms, speed);
				modelTypes["swarmerDie"]->BoneTransform(DyingTime, swarmerDieTransforms, speed);
			}
			else
				modelTypes["bossDie"]->BoneTransform(DyingTime, bossDieTransforms, speed);
		}
		else {
			DyingTime = 0.0f;
		}
		if (runningEnemies > 0) {
			RunningTime += dt_secs;
			if (currentLevel != BOSS) {
				modelTypes["assaultRun"]->BoneTransform(RunningTime, assaultRunTransforms, speed);
				modelTypes["enforcerRun"]->BoneTransform(RunningTime, enforcerRunTransforms, speed);
				modelTypes["swarmerRun"]->BoneTransform(RunningTime, swarmerRunTransforms, speed);
			}
			else
				modelTypes["bossRun"]->BoneTransform(RunningTime, bossRunTransforms, speed);
		}
		else {
			RunningTime = 0.0f;
		}
	}

	//TODO: delete
	void renderEditObject(glm::mat4 proj, Model *modelData, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation, GLuint shader, GLuint texture) {
		glm::mat4 model;
		model = glm::translate(model, pos);
		model = glm::rotate(model, float(rotation.y * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, scale);	// It's a bit too big for our scene, so scale it down

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		modelData->Draw(shader);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void renderWeapon(glm::mat4 proj, Model *modelData, GLuint shader, glm::vec3 scale, bool flip) {
		glDisable(GL_CULL_FACE);
		glm::mat4 model;
		float local_pitch = static_cast<float>(-(rotationAngles.x + 0.1));
		if (local_pitch >= 1)
			local_pitch = 1;
		if (local_pitch <= -1)
			local_pitch = -1;
		glm::vec3 gunPos;
		gunPos = shiftForward(glm::vec3(globalData->player->getPosition().x, globalData->player->getPosition().y-0.5, globalData->player->getPosition().z), rotationAngles, 0.075f);
		gunPos = shiftRight(gunPos, rotationAngles, 0.5f);
		model = glm::translate(model, gunPos);
		model = glm::rotate(model, -rotationAngles.y*DEG_TO_RADIAN, glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, local_pitch, glm::vec3(1.0, 0.0, 0.0));
		if (!flip)
			model = glm::rotate(model, float(80 * DEG_TO_RADIAN), glm::vec3(0.0, 1.0, 0.0));
		else
			model = glm::rotate(model, float(-100 * DEG_TO_RADIAN), glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model,scale);
		//I can't fix this. If you want, give it a try. good luck.
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(shader, "animated"), 0); //zero is no animations
		modelData->Draw(shader);
		glEnable(GL_CULL_FACE);

	}

	void updatePlayer(float dt) {
		globalData->player->update(dt);
	}

	void updateCollectables() { 
		unsigned int max = collectables.size();
		for (unsigned int i = 0; i < max; i++) {
			if (findCollision(get<0>(collectables[i]))) {
				if (get<2>(collectables[i]) == "heart")
					globalData->player->setHealth(globalData->player->getHealth() + 50);
				else if (get<2>(collectables[i]) == "shield")
					globalData->player->setArmor(globalData->player->getArmor() + 50);

				globalData->sound_manager->playSound(globalData->sound_manager->getSound(PICKUP_HEALTH), 2, 1);

				globalData->bt_manager->removeObject(bodies[get<1>(collectables[i])]);
				globalData->bt_manager->removeObject(get<0>(collectables[i]));
				bodies.erase(get<1>(collectables[i]));
				collectables.erase(remove(collectables.begin(), collectables.end(), collectables[i]), collectables.end());
				max--;
				//add something (hp / armor)
				//remove from world
			}
		}
	}

	glm::vec3 randomSpawnPoint() {
		srand(time(NULL));
		int num = std::rand() % 4 + 1; //random int between 1 and 4
		glm::vec3 spawnPoint1(0.0f, 0.0f, 0.0f);
		glm::vec3 spawnPoint2(0.0f, 0.0f, 0.0f);
		glm::vec3 spawnPoint3(0.0f, 0.0f, 0.0f);
		glm::vec3 spawnPoint4(0.0f, 0.0f, 0.0f);

		if (currentLevel == FIRST) {
			spawnPoint1 = glm::vec3(39.0f, 5.0f, 21.62f);
			spawnPoint2 = glm::vec3(20.3f, 5.0f, -19.17f);
			spawnPoint3 = glm::vec3(-36.7f, 5.0f, -33.79f);
			spawnPoint4 = glm::vec3(-34.9f, 5.0f, 35.02f);
		}
		else {
			spawnPoint1 = glm::vec3(29.6f, 5.0f, -31.02f);
			spawnPoint2 = glm::vec3(39.0f, 5.0f, 18.59f);
			spawnPoint3 = glm::vec3(-33.7f, 5.0f, 19.95f);
			spawnPoint4 = glm::vec3(-26.0f, 5.0f, -30.57f);
		}
		glm::vec3 def(0.0f, 0.0, 0.0f);

		if (num == 1)
			return spawnPoint1;
		if (num == 2)
			return spawnPoint2;
		if (num == 3)
			return spawnPoint3;
		if (num == 4)
			return spawnPoint4;
		else
			return def;
	}

	int killedEnemies = 0;
	double victory = 3;
	void update(SDL_Window * window, SDL_Event sdlEvent) {
		if (killedEnemies >= 10)
		{
			if (currentLevel == FIRST) { currentLevel = SECOND; }
			else if (currentLevel == SECOND) { currentLevel = BOSS; }
			//else { currentLevel = FIRST; }
			killedEnemies = 0;
			reset();
		}
		
		controls(window, sdlEvent);
		dt_secs = gameTime();
		coolDown -= dt_secs;	
		
		if (currentState == VICTORY) {
			victory -= dt_secs;
			if (victory <= 0)
			{
				reset();
				currentLevel = FIRST;
			}
		}
		if (currentState == RUNNING) {
			updatePlayer(dt_secs);
			if (globalData->player->getState() == JUMPING)
				if (findCollision(globalData->player->playerGhost))
					globalData->player->setState(ON_GROUND);
				
			updateCollectables();
			theta += 0.1f;
			globalData->bt_manager->update();
		}

		if (globalData->player->getLifeState() == DEAD)
		{
			currentState = DEFEAT;

			defeatTime -= dt_secs;
			if (defeatTime <= 0) {
				reset();
			}
		}

		if (currentLevel != BOSS) {
			if (enemies.size() < 6) {
				glm::vec3 spawnPoint = randomSpawnPoint();
				srand(time(NULL));
				int num = std::rand() % 4 + 1; //random int between 1 and 4
				if (num == 1) {
					enemies.insert(new Ranged(new NonPC(125, 6,
						globalData->bt_manager, spawnPoint,
						1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));
				}
				else if (num == 2) {
					enemies.insert(new Melee(new NonPC(200, 3,
						globalData->bt_manager, spawnPoint,
						1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));
				}
				else {
					enemies.insert(new Light(new NonPC(80, 3,
						globalData->bt_manager, spawnPoint,
						1.25, 0.5, 20, modelTypes["capsule"], modelProgram, defaultTexture, globalData->sound_manager)));

				}
			}
		}
	}

	void camera() {
		//		if (pointOfView == FIRST_PERSON) {
		at = moveForward(globalData->player->getPosition(), rotationAngles.y, 1.0f);
		at.y -= rotationAngles.x;

		view = glm::lookAt(globalData->player->getPosition(), at, up);
	}

	//function that passes all light positions and properties to the shader
	void pointLight(GLuint shader) {
		GLuint uniformIndex = glGetUniformLocation(shader, "viewPos");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(globalData->player->getPosition()));
		uniformIndex = glGetUniformLocation(shader, "pointLight.position");
		glUniform3f(uniformIndex, globalData->mainLight.position.x, globalData->mainLight.position.y, globalData->mainLight.position.z);
		uniformIndex = glGetUniformLocation(shader, "pointLight.ambient");
		glUniform3f(uniformIndex, globalData->mainLight.ambient.r, globalData->mainLight.ambient.g, globalData->mainLight.ambient.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.diffuse");
		glUniform3f(uniformIndex, globalData->mainLight.diffuse.r, globalData->mainLight.diffuse.g, globalData->mainLight.diffuse.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.specular");
		glUniform3f(uniformIndex, globalData->mainLight.specular.r, globalData->mainLight.specular.g, globalData->mainLight.specular.b);
		uniformIndex = glGetUniformLocation(shader, "pointLight.constant");
		glUniform1f(uniformIndex, globalData->mainLight.att_constant);
		uniformIndex = glGetUniformLocation(shader, "pointLight.linear");
		glUniform1f(uniformIndex, globalData->mainLight.att_linear);
		uniformIndex = glGetUniformLocation(shader, "pointLight.quadratic");
		glUniform1f(uniformIndex, globalData->mainLight.att_quadratic);
	}

	void pointShadow(GLuint shader) {
		glm::mat4 shadowProj = glm::perspective(float(90.0f*DEG_TO_RADIAN), aspect, near_plane, far_plane); //perspective projection is the best suited for this
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt((globalData->mainLight).position, globalData->mainLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		for (int k = 0; k < 6; ++k)
			glUniformMatrix4fv(glGetUniformLocation(shader, ("shadowMatrices[" + std::to_string(k) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[k]));
	}

	void renderWorldObjects(GLuint shader, glm::mat4 projection) {
		int tempAttacking = 0;
		int tempRunning = 0;
		int tempDying = 0;
		for (auto it = enemies.begin(); it != enemies.end(); ) {
			int speed = 0;
			string name = (*it)->getName();
			Model* currentModel = modelTypes[name + "Run"];
			
			if (name == "swarmer") { speed = 10; }
			else if (name == "boss") { speed = 7; }
			else { speed = 8; }

			if ((*it)->getState() == ATTACKING) {
				tempAttacking++;
				currentModel = modelTypes[name + "Attack"];
			}
			else if ((*it)->getState() == TRIGGERED) {
				tempRunning++;
				currentModel = modelTypes[name + "Run"];
			}
			else if ((*it)->getState() == DYING) {
				tempDying++;
				currentModel = modelTypes[name + "Die"];
			}

			SetBoneTransform((*it)->getState(), name);
			if (!(*it)->update(currentModel, view, projection, dt_secs, level1Grid, globalData->player, modelProgram, speed)) {
				it = enemies.erase(it);
				killedEnemies++;
				if (currentLevel == BOSS)
					currentState = VICTORY;
			}
			else { ++it; }
		}
		attackingEnemies = tempAttacking;
		runningEnemies = tempRunning;
		dyingEnemies = tempDying;

		for (const auto& id_pair : bodies) {
			// First = name / key
			id_pair.first; // string
			btVector3 p = bodies[id_pair.first]->getWorldTransform().getOrigin();
			btVector3 y = (((btBoxShape*)bodies[id_pair.first]->getCollisionShape())->getHalfExtentsWithMargin());
			glm::vec3 spherePosition = glm::vec3(p.x(), p.y(), p.z());
			glm::vec3 position = glm::vec3(p.x() + get<3>(models[id_pair.first]).x, p.y() - y.y() + get<3>(models[id_pair.first]).y, p.z() + get<3>(models[id_pair.first]).z);
			btQuaternion rotation = bodies[id_pair.first]->getWorldTransform().getRotation().normalized();

			if (id_pair.second->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE) {
				if (modelTypes[get<0>(models[id_pair.first])] == modelTypes["heart"] || modelTypes[get<0>(models[id_pair.first])] == modelTypes["shield"])
					renderObject(projection, modelTypes[get<0>(models[id_pair.first])], position, get<1>(models[id_pair.first]), rotation, shader, heartTexture, true);
				else
				{ 
					switch (currentLevel) {
					case FIRST:
						renderObject(projection, modelTypes[get<0>(models[id_pair.first])], position, get<1>(models[id_pair.first]), rotation, shader, groundTexture, false);
						break;
					case SECOND:
						renderObject(projection, modelTypes[get<0>(models[id_pair.first])], position, get<1>(models[id_pair.first]), rotation, shader, groundTexture_2, false);
						break;
					case BOSS:
						renderObject(projection, modelTypes[get<0>(models[id_pair.first])], position, get<1>(models[id_pair.first]), rotation, shader, groundTexture_3, false);
						break;
					}
				}
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {

				renderObject(projection, modelTypes[get<0>(models[id_pair.first])], spherePosition, get<1>(models[id_pair.first]), rotation, shader, defaultTexture, false);
			}

			if (id_pair.second->getCollisionShape()->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {

				renderObject(projection, modelTypes[get<0>(models[id_pair.first])], spherePosition, get<1>(models[id_pair.first]), rotation, shader, defaultTexture, false);
			}
		}

		if (mode == EDIT) {
			if (creation == true) {
				renderEditObject(projection, modelTypes[std::get<0>(temp[0])], std::get<1>(temp[0]), std::get<2>(temp[0]), std::get<3>(temp[0]), shader, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				renderEditObject(projection, modelTypes[std::get<0>(temp[1])], std::get<1>(temp[1]), std::get<2>(temp[1]), std::get<3>(temp[1]), shader, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDisable(GL_CULL_FACE);
			}
		}

		globalData->projectile_manager->renderProjectiles(view, projection, modelTypes["sphere"], shader, defaultTexture, dt_secs);

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
		glUniform3fv(uniformIndex, 1, glm::value_ptr(globalData->player->getPosition()));

		//similarly, if (!cubemap) refers to normal rendering
		if (!cubemap) {
			MeshManager::setUniformMatrix4fv(shader, "projection", glm::value_ptr(projection));
			MeshManager::setUniformMatrix4fv(shader, "view", glm::value_ptr(viewMatrix));
			// material properties in this case roughly translate to textures
			uniformIndex = glGetUniformLocation(shader, "material.diffuse");
			glUniform1i(uniformIndex, 0);
			uniformIndex = glGetUniformLocation(shader, "material.specular");
			glUniform1i(uniformIndex, 0);
			glUniform1f(glGetUniformLocation(shader, "material.shininess"), 32.0f); 
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, defaultTexture);
			// pass in the shadowmap
			uniformIndex = glGetUniformLocation(shader, "depthMap");
			glUniform1i(uniformIndex, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		}
		
		renderWorldObjects(shader, projection); //draw normal scene

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}

	void renderHud(GLuint shader, Model *modelData) {

		if (currentState == PAUSE)
			globalData->h_manager->renderPause(texturedProgram, modelTypes["cube"]);
		else if (currentState == MENU)
			globalData->h_manager->renderMenu(texturedProgram, modelTypes["cube"], globalData->shadows);
		else if (currentState == DEFEAT)
			globalData->h_manager->renderDefeat(texturedProgram, modelTypes["cube"]);
		else if (currentState == RUNNING) {
			// HP
			globalData->h_manager->renderPlayerHud("Health: ", globalData->player->getHealth(), HEALTH, shader, modelData, glm::vec3(-0.875f, 0.925f, 1.0f), glm::vec3(0.6275, 0.4, 0.0));
			// Armor
			globalData->h_manager->renderPlayerHud("Armor: ", globalData->player->getArmor(), ARMOR, shader, modelData, glm::vec3(-0.65f, 0.925f, 1.0f), glm::vec3(0, 0, 0.4));
			// Crosshair
			globalData->h_manager->renderCrosshair(texturedProgram, modelTypes["cube"]);
		}
		else if (currentState == VICTORY) {
			globalData->h_manager->renderVictory(texturedProgram, modelTypes["cube"]);
		}
	}

	void draw(SDL_Window * window) { 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window
		glEnable(GL_CULL_FACE);
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glm::mat4 projection(1.0);
		GLfloat scale(1.0f); // just to allow easy scaling of complete scene
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

				if (globalData->shadows)
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

				if (currentLevel == FIRST)
					skybox_2->renderSkybox(projection, view, modelTypes["cube"]);
				else if (currentLevel == SECOND)
					skybox->renderSkybox(projection, view, modelTypes["cube"]);
				else if (currentLevel == BOSS)
					skybox->renderSkybox(projection, view, modelTypes["cube"]);
				// normal rendering
				renderShadowedScene(projection, view, modelProgram, false); // render normal scene from normal point of view

				string wepType;
				float offsetRight = 0.5f;
				bool flip = false;
				glm::vec3 wepScale;
				if (globalData->player->getWeapon().getName() == PISTOL) {
					wepType = "scifipistol";
					wepScale = glm::vec3(0.25f, 0.25f, 0.25f);
					flip = false;
				}
				else if (globalData->player->getWeapon().getName() == NUKA) {
					wepType = "nukacola";
					wepScale = glm::vec3(0.003f, 0.004f, 0.003f);
					flip = false;
				}
				else if (globalData->player->getWeapon().getName() == SCIFI) {
					wepType = "scifigun";
					wepScale = glm::vec3(0.185f, 0.185f, 0.185f);
					flip = true;

				}

				animationTransforms();
	    	
				renderWeapon(projection, modelTypes[wepType], modelProgram, wepScale, flip); //TODO: Render current weapon

				renderHud(texturedProgram, modelTypes["cube"]);
				
				
				if (mode == EDIT) {
					globalData->h_manager->renderEditHud("Bounding", currentBounding, texturedProgram, modelTypes["cube"], glm::vec3(0.7f, 0.45f, 0.9f));
					globalData->h_manager->renderEditHud("Model", currentModel, texturedProgram, modelTypes["cube"], glm::vec3(0.7f, 0.35f, 0.9f));
					globalData->h_manager->renderEditHud("Mass", to_string(trunc(mass*10.0) / 10.0), texturedProgram, modelTypes["cube"], glm::vec3(0.7f, 0.25f, 0.9f));
					globalData->h_manager->renderEditHud("PrevAdd", selectedPrev, texturedProgram, modelTypes["cube"], glm::vec3(0.7f, -0.05f, 0.9f));
					string temp = "";
					if (stage == MODEL) {
						temp = "model";
					}
					else {
						temp = "bounding";
					}
					globalData->h_manager->renderEditHud("Editing", temp, texturedProgram, modelTypes["cube"], glm::vec3(0.7f, 0.15f, 0.9f));
					if (coolDown <= 0) {
						temp = "READY";
					}
					else {
						temp = "WAIT";
					}
					globalData->h_manager->renderEditHud("Timer", temp, texturedProgram, modelTypes["cube"], glm::vec3(0.7f, 0.05f, 0.9f));
				}
			}
			glDepthMask(GL_TRUE);
		}
		SDL_GL_SwapWindow(window); // swap buffers
	}
}
