#include "Player.h"


Player::Player(glm::vec3 _eye) {
	eye = _eye;
	health = 200;
	armor = 100;
	invincible = false;
	speed = SPEED_CAP_XZ;
}

void Player::update(float dt) {
	if (this->invincible)
	{
		invincibility_f -= dt;
		if (invincibility_f <= 0) {
			invincibility_f = INVINC_TIME;
			this->invincible = false;
		}
	}
}


void Player::setPosition(glm::vec3 newEye) {
	eye = newEye;
}
glm::vec3 Player::getPosition() {
	return eye;
}

playerState Player::getState(){
	return currentState;
}

void Player::setState(playerState newState) {
	currentState = newState;
}

double Player::getHealth() { return this->health; }
double Player::getArmor() { return this->armor; }
double Player::getSpeed() { return this->speed; }
void Player::setHealth(double hp) { this->health = hp; }
void Player::setArmor(double arm) { this->armor = arm; }
void Player::setSpeed(double sp) { this->speed = sp; }

void Player::takeDamage(double damage) {
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
