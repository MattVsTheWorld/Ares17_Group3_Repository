#include "Physics.h"

Physics::Physics() {

}

float Physics::applyGravity(glm::vec3 pos) {
	if (pos.y > GROUND_POSITION) pos.y -= gravity;
	return pos.y;
}