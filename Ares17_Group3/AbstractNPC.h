#ifndef ABSTRACTNPC
#define ABSTRACTNPC
#include "btShapeManager.h"
#include <iostream>
#include <iostream>

//+++ AI
#include "AdjacencyList.h"
#include "A_star.h"
#include <chrono>
#include "Grid.h"

using std::cout;
using std::endl;
using std::cin;

// Abstract base class: no implementation
class AbstractNPC {
public:
	//	virtual btRigidBody* addBoundingCapsule() = 0;
	virtual void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader) = 0;
	virtual void modifyHealth(double newHp) = 0;
	virtual void moveNpc(vertex* v) = 0;
	virtual queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId) = 0;
	virtual bool update(Model * modelData, glm::mat4 view, 
		glm::mat4 proj, float dt, Grid* _g, Player* player, GLuint shader) = 0;
	//	virtual bool findCollision() = 0;
	virtual void setAttack(double atk) = 0;
	virtual double getAttack() = 0;
	virtual double getRange() = 0;
	virtual void setRange(double rng) = 0;
	virtual double getHealth() = 0;
	virtual void setAttackSpeed(double atkspd) = 0;
	virtual	double getAttackSpeed() = 0;
	virtual ~AbstractNPC() {}
};


#endif