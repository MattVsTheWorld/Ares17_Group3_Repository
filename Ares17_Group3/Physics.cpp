#include "Physics.h"

Physics::Physics() {

}

// this needs to return a pair
vectorPair Physics::applyGravity(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc, float dt_secs) {
	//if (pos.y > GROUND_POSITION) { //needs improving

		glm::vec3 vel2(0.0, 0.0, 0.0);
		glm::vec3 pos2(0.0, 0.0, 0.0);
		glm::vec3 avg_vel(0.0, 0.0, 0.0);

			// euler's method 
			vel2.y = vel.y + acc.y * dt_secs; // next velocity
			avg_vel.y = (vel.y + vel2.y) / 2; // average velocoiy
			pos2.y = pos.y + avg_vel.y * dt_secs; // new position
	
			// update values
			pos.y = pos2.y;
			vel.y = vel2.y;
			//		cout << vel.y << endl;
	//	}
//	}
	//else vel.y = 0;
	/*
	else {
	pos.y += 0.05;
	vel.y = -vel.y;
	vel *= 1.0;
	//		cout << vel.y << endl;
	} */ // Simple bounciness
	return std::make_pair(pos, vel);
	// return position and velocity :thinking:
}

vectorPair Physics::applyPhysics(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc, float dt_secs) {
	
	glm::vec3 vel2(0.0, 0.0, 0.0);
	glm::vec3 pos2(0.0, 0.0, 0.0);
	glm::vec3 avg_vel(0.0, 0.0, 0.0);
	if (vel.x >= 0.1 || vel.x <= -0.1) {
				//acc.y = GRAVITY_VALUE;
		// euler's method
		// ensure acc is always friction

		if (vel.x >= 0 && acc.x <= 0)
			vel2.x = vel.x + acc.x * dt_secs; // next velocity
		else if (vel.x <= 0 && acc.x >= 0)
			vel2.x = vel.x + acc.x * dt_secs;
		else if (vel.x >= 0 && acc.x >= 0)
			vel2.x = vel.x - acc.x * dt_secs; // next velocity
		else if (vel.x <= 0 && acc.x <= 0)
			vel2.x = vel.x - acc.x * dt_secs;

		avg_vel.x = (vel.x + vel2.x) / 2; // average velocoiy
		pos2.x = pos.x + avg_vel.x * dt_secs; // new position

		pos.x = pos2.x;
		vel.x = vel2.x;
	} else vel.x = 0.0;

		if (vel.z >= 0.1 || vel.z <= -0.1) {
			// ensure acc is always friction
			if (vel.z >= 0 && acc.z <= 0)
				vel2.z = vel.z + acc.z * dt_secs; // next velocity
			else if (vel.z <= 0 && acc.z >= 0)
				vel2.z = vel.z + acc.z * dt_secs;
			else if (vel.z >= 0 && acc.z >= 0)
				vel2.z = vel.z - acc.z * dt_secs; // next velocity
			else if (vel.z <= 0 && acc.z <= 0)
				vel2.z = vel.z - acc.z * dt_secs;

			avg_vel.z = (vel.z + vel2.z) / 2; // average velocoiy
			pos2.z = pos.z + avg_vel.z * dt_secs; // new position

			pos.z = pos2.z;
			vel.z = vel2.z;
		}
		else vel.z = 0.0;
	
	return std::make_pair(pos, vel);

}
