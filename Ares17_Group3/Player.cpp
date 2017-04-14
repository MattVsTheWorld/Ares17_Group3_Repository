#include "Player.h"


Player::Player(glm::vec3 _eye, float rad, float height, float mass, btShapeManager *_btMan) {
	eye = _eye;
	health = 200;
	armor = 100;
	invincible = false;
	speed = SPEED_CAP_XZ;
	vitState = ALIVE;
	availableWeapons.push_back(Weapon(PISTOL, 20));
	availableWeapons.push_back(Weapon(NUKA, 10));
	availableWeapons.push_back(Weapon(SCIFI, 40));
	
	currentWep = Weapon(PISTOL, 10);

	bt_manager = _btMan;

	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(this->eye.x, this->eye.y, this->eye.z));
	btCapsuleShape* playerShape = new btCapsuleShape(rad, height);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		playerShape->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, playerShape, inertia);

	playerBody = new btRigidBody(info);
	playerBody->setAngularFactor(0); // Doesn't fall sideways
	int playerCollidesWith = COL_DEFAULT | COL_ENEMY | COL_COLLECTABLE;
	// body, group, mask
	bt_manager->addToWorld(playerBody, COL_PLAYER, playerCollidesWith);
	playerBody->setActivationState(DISABLE_DEACTIVATION);
	playerBody->setFriction(FRICTION);

	// Now ghost
	playerGhost = new btPairCachingGhostObject();								// create object
	playerGhost->setCollisionShape(playerShape);								// set shape
	playerGhost->setWorldTransform(t);											// set world transform	
	playerGhost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);  // disable collision response for ghost

	bt_manager->addGhostToWorld(playerGhost, COL_PLAYER, playerCollidesWith);
}

void Player::update(float dt) {
	if (this->invincible)
	{
		invincibility_f -= dt;				//Invincibility frames
		if (invincibility_f <= 0) {
			invincibility_f = INVINC_TIME;
			this->invincible = false;
		}
	}
	if (this->health >= HEALTH_CAP)
		this->health = HEALTH_CAP;
	if (this->armor >= ARMOR_CAP)
		this->armor = ARMOR_CAP;
	if (this->health <= 0)
		this->vitState = DEAD;
	if (this->armor < 0)
		this->armor = 0;

	btTransform t;
	t.setIdentity();
	playerBody->getMotionState()->getWorldTransform(t);
	btVector3 pos = t.getOrigin();
	this->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
	playerGhost->setWorldTransform(t);
}


void Player::setPosition(glm::vec3 newEye) {
	eye = newEye;
}
glm::vec3 Player::getPosition() {
	return eye;
}

playerState Player::getState(){
	return currentState;
}

void Player::setState(playerState newState) {
	currentState = newState;
}

double Player::getHealth() { return this->health; }
double Player::getArmor() { return this->armor; }
double Player::getSpeed() { return this->speed; }
lifeState Player::getLifeState() { return this->vitState; }
void Player::setHealth(double hp) { this->health = hp; }
void Player::setArmor(double arm) { this->armor = arm; }
void Player::setSpeed(double sp) { this->speed = sp; }

void Player::setWeapon(wepType newWep) { 
	for (auto i : availableWeapons)
		if (i.getName() == newWep)
			this->currentWep = i;
	
	
}
Weapon Player::getWeapon() { return this->currentWep; }

void Player::takeDamage(double damage) {
	if (!this->invincible) {
		this->invincible = true;
		if (this->armor > 0)
			this->armor -= (damage / 2);
		else {
			this->health -= damage;
			if (this->health <= 0)
				this->health = 0;
		}
	}

}

void Player::restart() {
	this->vitState = ALIVE;
	this->health = 200;
	this->armor = 100;
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(2.0f, 3.0f, -6.0f));
	playerBody->setWorldTransform(t);
}
