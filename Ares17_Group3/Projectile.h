#ifndef PROJECTILE
#define PROJECTILE

#include "btShapeManager.h"
#define BULLET_SIZE 0.05f
#define DEFAULT_LIFESPAN 6.0f
#define DEFAULT_MASS 1.0f
#define PROJ_SPEED 100.0f
#define PITCHLOCK 3.0f
#define COOL_TIME 0.2f

typedef tuple<btRigidBody*, double, btPairCachingGhostObject*> _proj;

class Projectile {
private:
	btShapeManager *shapeManager;
	vector <_proj> liveProjectiles;
	bool findCollision(btPairCachingGhostObject* ghostObject);
protected:
public:
	Projectile(btShapeManager *s_Manager);
	btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	void addProjectile(glm::vec3 spawn, float speed, float yaw, float pitch);
	void renderProjectiles(glm::mat4 view, glm::mat4 proj, Model * modelData, 
		GLuint shader, GLuint texture, double time_step);
	
};


#endif