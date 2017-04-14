#ifndef ABSTRACTNPC
#define ABSTRACTNPC
#include "btShapeManager.h"
#include "SoundManager.h"
#include <iostream>
#include <iostream>

//AI
#include "AdjacencyList.h"
#include "A_star.h"
#include <chrono>
#include "Grid.h"
#include "Player.h"

using std::cout;
using std::endl;
using std::cin;

enum npcState {
	IDLE,		//Hasn't seen player
	TRIGGERED,	//Player seen
	PAUSED,		//Game paused
	DYING,
	ATTACKING
};

// Abstract base class: no implementation
class AbstractNPC {
public:
	virtual void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player) = 0;
	virtual void modifyHealth(double newHp) = 0;
	virtual void moveNpc(vertex* v, float speed) = 0;
	virtual queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId) = 0;
	virtual bool update(Model* modelData, glm::mat4 view,
		glm::mat4 proj, float dt, Grid* _g, Player* player, GLuint shader, float speed) = 0;
	virtual void setAttack(double atk) = 0;
	virtual double getAttack() = 0;
	virtual double getRange() = 0;
	virtual void setRange(double rng) = 0;
	virtual double getHealth() = 0;
	virtual void setAttackSpeed(double atkspd) = 0;
	virtual	double getAttackSpeed() = 0;
	virtual void setState(npcState newState) = 0;
	virtual npcState getState() = 0;
	virtual void setName(string _name) = 0;
	virtual string getName() = 0;
	virtual ~AbstractNPC() {}
};


#endif