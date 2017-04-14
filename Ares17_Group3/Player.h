#ifndef PLAYER
#define PLAYER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "btShapeManager.h"
#include "Weapon.h"

#define INVINC_TIME 0.5f

#define SPEED_CAP_XZ 8.0
#define FRICTION 5.6
#define SPEED_CAP_Y 2.5f

#define HEALTH_CAP 300
#define ARMOR_CAP 100

enum playerState { ON_GROUND, JUMPING };
enum lifeState { ALIVE, DEAD };

class Player {
public:
	Player(glm::vec3 _eye, float rad, float height, float mass, btShapeManager *_btMan);
	void update(float dt);
	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
	playerState getState();
	void setState(playerState newState);
	double getHealth();
	double getArmor();
	double getSpeed();
	lifeState getLifeState();
	void setWeapon(wepType newWep);
	Weapon getWeapon();
	void setHealth(double hp);
	void setArmor(double arm);
	void takeDamage(double damage);
	void setSpeed(double sp);

	void restart();

	btRigidBody* playerBody;

	btPairCachingGhostObject* playerGhost;

protected:

private:
	glm::vec3 eye;
	playerState currentState;
	lifeState vitState;
	Weapon currentWep;
	vector<Weapon> availableWeapons;
	double speed;
	double health;
	double armor;
	bool invincible;
	float invincibility_f;

	btShapeManager *bt_manager;


};

#endif