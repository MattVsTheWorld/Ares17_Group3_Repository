#ifndef COLLISIONS
#define COLLISIONS

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef std::pair < glm::vec3, glm::vec3 > vectorPair;

class Collisions {
public:
	Collisions();
	bool doCollisions(glm::vec3 position_A, glm::vec3 scale_A, glm::vec3 position_B, glm::vec3 scale_B);

	vectorPair elastic();

protected:
private:

};
#endif