#include "Physics.h"

Physics::Physics() {

}

// EXTREMELY simplistic gravity; will modify it once I get home
/*
float Physics::applyGravity(glm::vec3 pos) {
	if (pos.y > GROUND_POSITION) pos.y -= gravity;
		currentTime = clock();
	if (currentTime > lastTime + DT_MILLISECONDS) { // operations done every ~33ms
	//	printf("Diff: %d\n", currentTime - lastTime);

	unsigned int dt = currentTime - lastTime;
	float dt_secs = (float)dt / 1000;
	std::cout << dt_secs << std::endl;
	lastTime = currentTime;
	//printf()
	} 
	return pos.y;
} */

// this needs to return a pair
vectorPair Physics::applyGravity(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc, float dt_secs) {
	if (pos.y > GROUND_POSITION) { //pos.y -= gravity;

		glm::vec3 vel2(0.0, 0.0, 0.0);
		glm::vec3 pos2(0.0, 0.0, 0.0);
		glm::vec3 avg_vel(0.0, 0.0, 0.0);
//		currentTime = clock();
	//	if (currentTime > lastTime + DT_MILLISECONDS) { // operations done every ~33ms
														//	printf("Diff: %d\n", currentTime - lastTime);

		//	unsigned int dt = currentTime - lastTime;
		//	float dt_secs = (float)dt / 1000;
		//	if (dt_secs > 0.017) dt_secs = 0.017; // first value is off ( 5.5~)
		//	std::cout << dt_secs << std::endl;
		//	lastTime = currentTime;
			//acc.y = GRAVITY_VALUE;
			// euler's method 
			vel2.y = vel.y + acc.y * dt_secs; // next velocity
			avg_vel.y = (vel.y + vel2.y) / 2; // average velocoiy
			pos2.y = pos.y + avg_vel.y * dt_secs; // new position
		//	cout << "vel " << vel.y << " vel2 " << vel2.y << " avg " << avg_vel.y << " dt " << dt_secs << " pos " << pos.y << " pos2 " << pos2.y << endl;
			
			// same for x component
			//	cout << avg_vel.y << endl;
			//	bodies[i]->moveTo(pos2.x, pos2.y); // move to new position
			// update values
			pos.y = pos2.y;
			vel.y = vel2.y;
			//		cout << vel.y << endl;
	//	}
	}
	else vel.y = 0;
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
