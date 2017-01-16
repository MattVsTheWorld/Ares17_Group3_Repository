#include "Collisions.h"


Collisions::Collisions(){}

// axis aligned
// do Oriented bounding boxes?

bool Collisions::doCollisions(glm::vec3 position_A, glm::vec3 scale_A, glm::vec3 position_B, glm::vec3 scale_B) {

	bool collision = false;

	if ((position_A.x + (scale_A.x)) >= (position_B.x - (scale_B.x)) // if right side on the right of left side
		&& position_A.x - (scale_A.x) <= (position_B.x + (scale_B.x))) // and left side is left of right side
	{
		// if possible collision in x, then check z
		if ((position_A.z + (scale_A.z)) >= (position_B.z - (scale_B.z)) // if right side on the right of left side
			&& position_A.z - (scale_A.z) <= (position_B.z + (scale_B.z))) // and left side is left of right side
		{
			if ((position_A.y + (scale_A.y)) >= (position_B.y - (scale_B.y))
				&& position_A.y - (scale_A.y) <= (position_B.y + (scale_B.y)))
				collision = true;
		}
	}

	return collision;

}


// elastic rebound
/** [2d] velocity, parallel velocity, perpendicular velocity, surface normal
	vel = vel.par + vel.perp
	vel.perp = (vel.par)normal
	vel.par = vel - v.perp
	~~
	elastic rebound
	newVel = v.par - v.perp
	~~
	[3d] same but use x and z and keep y (??) (or dampen)
	[slide] newVel = v.par
*/

vectorPair Collisions::elastic() {
	return std::make_pair(glm::vec3(), glm::vec3());
}