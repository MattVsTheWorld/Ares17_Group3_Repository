#ifndef PLAYER
#define PLAYER
#include "Object.h"

enum States { ON_GROUND, JUMPING };

class Player : public Object {
public:
	Player(transformation_Matrices transformation, object_Properties obj_p);
	Player(transformation_Matrices transformation, char *texturePath, object_Properties obj_p);
	Player(transformation_Matrices transformation);
	States getState();
	void setState(States state);
protected:

private:
	States player_state = JUMPING; // because of initial player pos

};

#endif