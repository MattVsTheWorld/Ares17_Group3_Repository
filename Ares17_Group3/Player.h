#ifndef PLAYER
#define PLAYER
//#include "Object.h"

//enum States { ON_GROUND, JUMPING };

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Player {
public:
//	Player(transformation_Matrices transformation, object_Properties obj_p);
//	Player(transformation_Matrices transformation, char *texturePath, object_Properties obj_p);
	Player(glm::vec3 _eye);

	void setPosition(glm::vec3 newEye);
	glm::vec3 getPosition();
//	States getState();
	//void setState(States state);
protected:

private:
	glm::vec3 eye;
//	transformation_Matrices trans_m;
//	States player_state = JUMPING; // because of initial player pos

};

#endif