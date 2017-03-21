#include "Player.h"


Player::Player(glm::vec3 _eye) {
	eye = _eye;
	health = 200;
	armor = 100;
	invincible = false;
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