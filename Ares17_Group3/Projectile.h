#ifndef PROJECTILE
#define PROJECTILE

#include "btShapeManager.h"
#define BULLET_SIZE 0.05f
#define DEFAULT_LIFESPAN 2.0f
#define DEFAULT_MASS 1.0f
#define PROJ_SPEED 2.0f

typedef pair<btRigidBody*, double> _proj;

class Projectile{
private:
	btShapeManager *shapeManager;
	vector <pair<btRigidBody*, double>> *liveProjectiles;
protected:
public:
	Projectile(btShapeManager *s_Manager) {
		this->shapeManager = s_Manager;
		liveProjectiles = new vector <_proj>;
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
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, GLuint shader, GLuint texture, double time_step) {
		if (liveProjectiles->size() > 0) {
		//	using _proj = pair<btRigidBody*, double*>;
			//cout << time_step << " ";
			for (vector <_proj>::iterator projectileIterator = liveProjectiles->begin(); projectileIterator != liveProjectiles->end(); ++projectileIterator)
			{
				(static_cast<_proj>(*projectileIterator)) = make_pair((static_cast<_proj>(*projectileIterator)).first, (static_cast<_proj>(*projectileIterator)).second - time_step);
				cout << (static_cast<_proj>(*projectileIterator)).second << " ";
				if ((static_cast<_proj>(*projectileIterator)).second <= 0) // if dead, remove // delete?
					liveProjectiles->erase(remove(liveProjectiles->begin(), liveProjectiles->end(), static_cast<_proj>(*projectileIterator)), liveProjectiles->end());
				else {
					this->shapeManager->renderSphere(((static_cast<_proj>(*projectileIterator)).first), view, proj, modelData, shader, texture);//		this->shapeManager->renderSphere((((pair<btRigidBody*, double>)*projectileIterator).first)
				}
			}
		}
	}
};

#endif