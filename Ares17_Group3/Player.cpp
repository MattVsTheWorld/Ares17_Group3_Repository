#include "Player.h"

Player::Player(transformation_Matrices transformation, object_Properties obj_p) : Object(transformation, obj_p) {}
Player::Player(transformation_Matrices transformation, char *texturePath, object_Properties obj_p) : Object(transformation, texturePath, obj_p) {}

Player::Player(transformation_Matrices transformation) : Object(transformation) {}

States Player::getState() {
	return player_state;
}

void Player::setState(States state) {
	player_state = state;
}
	//	scale = scal;