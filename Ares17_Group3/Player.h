#ifndef PLAYER
#define PLAYER
//http://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet/25725502#25725502
// might be useful

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define INVINC_TIME .75f

enum playerState { ON_GROUND, JUMPING };

class Player {
public:
	Player(glm::vec3 _eye);
	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
	playerState getState();
	void setState(playerState newState);

	double getHealth() { return this->health; }
	double getArmor() { return this->armor; }
	void varyHealth(double hp) { this->health += hp; }
	void setHealth(double hp) { this->health = hp; }
	void varyArmor(double arm) { this->armor += arm; }
	void setArmor(double arm) { this->armor = arm; }
protected:

private:
	glm::vec3 eye;
	playerState currentState;
	double health;
	double armor;

};

#endif