#include "Bullet.h"


Bullet::Bullet(transformation_Matrices transformation, object_Properties obj_p) : Object(transformation, obj_p) {}
Bullet::Bullet(transformation_Matrices transformation, char *texturePath, object_Properties obj_p) : Object(transformation, texturePath, obj_p) {}

//get and set angles at shot
float Bullet::getPitchAngle() { return pitchAngleAtShot; }
void Bullet::setPitchAngle(float angle) { pitchAngleAtShot = angle; }
float Bullet::getYawAngle() { return yawAngleAtShot; }
void Bullet::setYawAngle(float angle) { yawAngleAtShot = angle; }
float Bullet::getRollAngle() { return rollAngleAtShot; }
void Bullet::setRollAngle(float angle) { rollAngleAtShot = angle; }