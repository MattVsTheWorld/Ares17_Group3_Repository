#ifndef PLAYER
#define PLAYER
//http://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet/25725502#25725502
// might be useful

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define INVINC_TIME 0.5f

enum playerState { ON_GROUND, JUMPING };

class Player {
public:
	Player(glm::vec3 _eye);

	void update(float dt) {
		if (this->invincible)
		{
			invincibility_f -= dt;
			if (invincibility_f <= 0) {
				invincibility_f = INVINC_TIME;
				this->invincible = false;
	//			std::cout << "NOT ANYMORE" << std::endl;
			}
		}
	}

	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
	playerState getState();
	void setState(playerState newState);

	double getHealth() { return this->health; }
	double getArmor() { return this->armor; }
	void setHealth(double hp) { this->health = hp; }
	void setArmor(double arm) { this->armor = arm; }
	void takeDamage(double damage) {
		if (!this->invincible)
		{
			this->invincible = true;
	//		std::cout << "NO DAMGIO" << std::endl;
			if (this->armor > 0)
				this->armor -= (damage / 2);
			else
				this->health -= damage;
		//	std::cout << "Took a hit! Hp now: " << this->health << std::endl;
		}
		
	}
protected:

private:
	glm::vec3 eye;
	playerState currentState;
	double health;
	double armor;
	bool invincible;
	float invincibility_f;

};

#endif