#include "Player.h"


Player::Player(glm::vec3 _eye) {
		eye = _eye;
}

void Player::setPosition(glm::vec3 newEye) {
	eye = newEye;
}
glm::vec3 Player::getPosition() {
	return eye;
}

/*
States Player::getState() {
	return player_state;
}

void Player::setState(States state) {
	player_state = state;
}
	//	scale = scal;

	*/