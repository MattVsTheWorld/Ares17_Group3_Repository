#ifndef PROJECTILE
#define PROJECTILE

#include "btShapeManager.h"
#include "TempObjects.h"

class Projectile : public TempObjects {
private:
	double lifespan;
	
	btShapeManager *shapeManager;
protected:
public:
	Projectile(btShapeManager *s_Manager) {
		this->shapeManager = s_Manager;
		liveProjectiles = new vector <pair<btRigidBody*, double>>;
		//liveProjectiles.push_back(make_pair(shapeManager->addSphere(10, 0, 0, 0, 10), this->lifespan));
	}
	void addProjectile(glm::vec3 direction, float speed) {
		liveProjectiles->push_back(make_pair(shapeManager->addSphere(2, 0, 2, 0, 10), 20));
		//cout << liveProjectiles->size();
	}
	~Projectile() {
		delete this; // :thinking:
	}
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture) {
		if (liveProjectiles->size() > 0) {
			for (vector <pair<btRigidBody*, double>>::iterator projectileIterator = liveProjectiles->begin(); projectileIterator != liveProjectiles->end(); ++projectileIterator)
			{
				if (((pair<btRigidBody*, double>)*projectileIterator).second <= 0) // if dead, remove // delete?
					liveProjectiles->erase(remove(liveProjectiles->begin(), liveProjectiles->end(), (pair<btRigidBody*, double>)*projectileIterator), liveProjectiles->end());
				else {
					this->shapeManager->renderSphere((((pair<btRigidBody*, double>)*projectileIterator).first), view, proj, modelData, shader, texture);//		this->shapeManager->renderSphere((((pair<btRigidBody*, double>)*projectileIterator).first)
				}
			}
		}
	}
};

#endif