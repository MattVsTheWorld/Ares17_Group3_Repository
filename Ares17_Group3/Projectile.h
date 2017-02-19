#ifndef PROJECTILE
#define PROJECTILE
#include "btShapeManager.h"

class Projectile {
private:
	double lifespan;
	static vector <pair<btRigidBody*, double>> liveProjectiles;
	btShapeManager *shapeManager;
protected:
public:
	Projectile(btShapeManager *s_Manager, glm::vec3 direction, float speed) {
		this->shapeManager = s_Manager;
		liveProjectiles.push_back(make_pair(shapeManager->addSphere(10, 0, 0, 0, 10), this->lifespan));
	}
	~Projectile() {
		delete this; // :thinking:
	}
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture) {
		for (vector <pair<btRigidBody*, double>>::iterator projectileIterator = this->liveProjectiles.begin(); projectileIterator != this->liveProjectiles.end(); ++projectileIterator)
		{
			if (((pair<btRigidBody*, double>)*projectileIterator).second <= 0) // if dead, remove
				liveProjectiles.erase(remove(liveProjectiles.begin(), liveProjectiles.end(), (pair<btRigidBody*, double>)*projectileIterator), liveProjectiles.end());
			else {
				;//		this->shapeManager->renderSphere((((pair<btRigidBody*, double>)*projectileIterator).first)
			}
		}
		
	}

};

#endif