#include "Physics.h"

Physics::Physics() {

}

// EXTREMELY simplistic gravity; will modify it once I get home
float Physics::applyGravity(glm::vec3 pos) {
	if (pos.y > GROUND_POSITION) pos.y -= gravity;
	return pos.y;
}

/*
float Physics::applyGravity(glm::vec3 pos) {
	if (pos.y > GROUND_POSITION) {
		pos.y -= gravity;
	}
	return pos.y;
}
*/