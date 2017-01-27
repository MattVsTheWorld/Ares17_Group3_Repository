#ifndef PLAYER
#define PLAYER
//http ://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet/25725502#25725502
// might be useful

// Probably need bullet


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Player {
public:
	Player(glm::vec3 _eye);
	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
protected:

private:
	glm::vec3 eye;
};

#endif