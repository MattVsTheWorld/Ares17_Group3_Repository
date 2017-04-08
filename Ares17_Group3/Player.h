#ifndef PLAYER
#define PLAYER
//http://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet/25725502#25725502
// might be useful

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "btShapeManager.h"

#define INVINC_TIME 0.5f

#define SPEED_CAP_XZ 8.0
#define FRICTION 5.6
#define SPEED_CAP_Y 2.5f

#define HEALTH_CAP 300
#define ARMOR_CAP 100

enum playerState { ON_GROUND, JUMPING };
enum lifeState { ALIVE, DEAD };
enum equippedWep { PISTOL, NUKA, RIFLE };

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
	void setWeapon(equippedWep newWep);
	equippedWep getWeapon();
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
	equippedWep currentWep;
	double speed;
	double health;
	double armor;
	bool invincible;
	float invincibility_f;

	btShapeManager *bt_manager;


};

#endif