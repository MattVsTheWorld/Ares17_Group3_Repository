#include "Physics.h"

Physics::Physics() {

}

// EXTREMELY simplistic gravity; will modify it once I get home
float Physics::applyGravity(glm::vec3 pos) {
	if (pos.y > GROUND_POSITION) pos.y -= gravity;
	/*	currentTime = clock();
	if (currentTime > lastTime + DT_MILLISECONDS) { // operations done every ~33ms
	//	printf("Diff: %d\n", currentTime - lastTime);

	unsigned int dt = currentTime - lastTime;
	float dt_secs = (float)dt / 1000;
	std::cout << dt_secs << std::endl;
	lastTime = currentTime;
	//printf()
	} */
	return pos.y;
}

// this needs to return a pair
vectorPair Physics::applyGravity(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc) {
	if (pos.y > GROUND_POSITION) { //pos.y -= gravity;

		glm::vec3 vel2(0.0, 0.0, 0.0);
		glm::vec3 pos2(0.0, 0.0, 0.0);
		glm::vec3 avg_vel(0.0, 0.0, 0.0);
		currentTime = clock();
	//	if (currentTime > lastTime + DT_MILLISECONDS) { // operations done every ~33ms
														//	printf("Diff: %d\n", currentTime - lastTime);

			unsigned int dt = currentTime - lastTime;
			float dt_secs = (float)dt / 1000;
			if (dt_secs > 0.017) dt_secs = 0.017; // first value is off ( 5.5~)
		//	std::cout << dt_secs << std::endl;
			lastTime = currentTime;

			//acc.y = GRAVITY_VALUE;


			// euler's method
			vel2.y = vel.y + acc.y * dt_secs; // next velocity
			avg_vel.y = (vel.y + vel2.y) / 2; // average velocoiy
			pos2.y = pos.y + avg_vel.y * dt_secs; // new position
												  // same for x component
												  //	cout << avg_vel.y << endl;


												  //	bodies[i]->moveTo(pos2.x, pos2.y); // move to new position
												  // update values
			pos.y = pos2.y;
			vel.y = vel2.y;


			//		cout << vel.y << endl;
	//	}
	}
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

/*
vectorPair Physics::applyGravity(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc) {
//	if (pos.y > GROUND_POSITION) pos.y -= gravity;

glm::vec3 vel2(0.0 ,0.0, 0.0);
glm::vec3 pos2(0.0, 0.0, 0.0);
glm::vec3 avg_vel(0.0, 0.0, 0.0);
currentTime = clock();
if (currentTime > lastTime + DT_MILLISECONDS) { // operations done every ~33ms
//	printf("Diff: %d\n", currentTime - lastTime);

unsigned int dt = currentTime - lastTime;
float dt_secs = (float)dt / 1000;
std::cout << dt_secs << std::endl;
lastTime = currentTime;

// accelerations
// 		acc.y += ((G * mass[k]) / (pow(r, 2) + pow(epsilon, 2)) * sin(angle));  // y component
//		acc.x += ((G * mass[k]) / (pow(r, 2) + pow(epsilon, 2)) * cos(angle)); // x component
acc.y += 0;
acc.x += 0;
acc.z += 0;

// euler's method
vel2.y = vel.y + acc.y * dt_secs; // next velocity
avg_vel.y = (vel.y + vel2.y) / 2; // average velocoiy
pos2.y = pos.y + avg_vel.y * dt_secs; // new position
// same for x component
vel2.x = vel.x + acc.x * dt_secs;
avg_vel.x = (vel.x + vel2.x) / 2;
pos2.x = pos.x + avg_vel.x * dt_secs;

vel2.z = vel.z + acc.z * dt_secs;
avg_vel.z = (vel.z + vel2.z) / 2;
pos2.z = pos.z + avg_vel.z * dt_secs;

//	bodies[i]->moveTo(pos2.x, pos2.y); // move to new position
// update values
pos.y = pos2.y;
vel.y = vel2.y;

pos.x = pos2.x;
vel.x = vel2.x;

pos.z = pos2.z;
vel.z = vel2.z;


}
return std::make_pair(pos,vel);
// return position and velocity :thinking:
}*/
