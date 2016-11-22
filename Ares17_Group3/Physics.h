#ifndef PHYSICS
#define PHYSICS

// check if all of these are used
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// debug
#include <iostream>
#include <time.h>

using namespace std;
#define GRAVITY_VALUE 0.005
#define GROUND_POSITION 0.5
#define DT_MILLISECONDS 20
// this seems to give a time step of 0.033 seconds
// Seems strange, will have to check if reliable


class Physics {
private:
	const float gravity = GRAVITY_VALUE;
	// debug
	unsigned int lastTime = 0, currentTime;
protected:
public:
	Physics();
	// simplistic definition	
	float applyGravity(glm::vec3 pos);
	float applyGravity(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc);


};
#endif