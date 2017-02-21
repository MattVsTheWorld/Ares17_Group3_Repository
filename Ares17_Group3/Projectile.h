#ifndef PROJECTILE
#define PROJECTILE

#include "btShapeManager.h"
#include "TempObjects.h"
#define BULLET_SIZE 0.05f
#define DEFAULT_LIFESPAN 20.0f
#define DEFAULT_MASS 1.0f

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
	void addProjectile(glm::vec3 spawn, float speed, float yaw, float pitch) {
		btRigidBody *temp = shapeManager->addSphere(BULLET_SIZE, spawn.x, spawn.y, spawn.z, DEFAULT_MASS);
		liveProjectiles->push_back(make_pair(temp, DEFAULT_LIFESPAN));
		// TODO: fix angles. yaw and pitch are ""unrelated"", creating the unwanted effect that shooting up won't reduce ground-parallel speed
		temp->setLinearVelocity(btVector3(speed*std::sin(yaw), -speed*std::sin(pitch), -speed*std::cos(yaw)));
		//cout << liveProjectiles->size();
	}
	~Projectile() {
		delete this; // :thinking:
	}
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture) {
		if (liveProjectiles->size() > 0) {
			using _proj = pair<btRigidBody*, double>;
			for (vector <_proj>::iterator projectileIterator = liveProjectiles->begin(); projectileIterator != liveProjectiles->end(); ++projectileIterator)
			{
				if (((_proj)*projectileIterator).second <= 0) // if dead, remove // delete?
					liveProjectiles->erase(remove(liveProjectiles->begin(), liveProjectiles->end(), static_cast<_proj>(*projectileIterator)), liveProjectiles->end());
				else {
					this->shapeManager->renderSphere(((static_cast<_proj>(*projectileIterator)).first), view, proj, modelData, shader, texture);//		this->shapeManager->renderSphere((((pair<btRigidBody*, double>)*projectileIterator).first)
				}
			}
		}
	}
};

#endif