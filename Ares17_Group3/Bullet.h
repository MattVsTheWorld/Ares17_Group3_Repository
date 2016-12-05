#ifndef BULLET
#define BULLET
#include "Object.h"

class Bullet : public Object {
private:
	//initial pitch,yaw,angle, of object
	float pitchAngleAtShot;
	float yawAngleAtShot;
	float rollAngleAtShot;

protected:

public:
	Bullet(transformation_Matrices transformation, object_Properties obj_p);
	Bullet(transformation_Matrices transformation, char *texturePath, object_Properties obj_p);

	//get and set angles at shot
	float getPitchAngle();
	void setPitchAngle(float angle);
	float getYawAngle();
	void setYawAngle(float angle);
	float getRollAngle();
	void setRollAngle(float angle);
};
#endif
