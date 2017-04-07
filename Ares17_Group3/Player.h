#ifndef PLAYER
#define PLAYER
//http://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet/25725502#25725502
// might be useful

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define INVINC_TIME 0.5f

#define SPEED_CAP_XZ 8.0
#define FRICTION 6.0
#define SPEED_CAP_Y 2.25f

#define HEALTH_CAP 300
#define ARMOR_CAP 100

enum playerState { ON_GROUND, JUMPING };

class Player {
public:
	Player(glm::vec3 _eye);
	void update(float dt);
	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
	playerState getState();
	void setState(playerState newState);
	double getHealth();
	double getArmor();
	double getSpeed();
	void setHealth(double hp);
	void setArmor(double arm);
	void takeDamage(double damage);
	void setSpeed(double sp);
protected:

private:
	glm::vec3 eye;
	playerState currentState;
	double speed;
	double health;
	double armor;
	bool invincible;
	float invincibility_f;

};

#endif