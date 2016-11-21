#ifndef PHYSICS
#define PHYSICS

// check if all of these are used
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
#define GRAVITY_VALUE 0.005
#define GROUND_POSITION 0.5


class Physics {
private:
	const float gravity = GRAVITY_VALUE;
	// 
protected:
public:
	Physics();
	// simplistic definition	
	float applyGravity(glm::vec3 pos);


};
#endif