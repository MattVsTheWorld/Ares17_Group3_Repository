#ifndef NPCDECORATOR
#define NPCDECORATOR
#include "NonPC.h"

// NPCDecorator implements AbstractNPC
class NPCDecorator : public AbstractNPC {
private:
	float timer = 0.7f;
public:
	NPCDecorator(AbstractNPC *n);
	~NPCDecorator();

	void moveNpc(vertex* v, float speed);
	queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId);

	bool update(Model* modelData, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player* player, GLuint shader, float speed);
	void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player);

	void modifyHealth(double newHp);	
	void setAttack(double atk);
	double getAttack();
	void setRange(double rng);
	double getRange();
	double getHealth();
	void setAttackSpeed(double atkspd);
	double getAttackSpeed();
	void setState(npcState newState);
	npcState getState();
	string getName();
	void setName(string name);
protected:
	AbstractNPC * npc;
};

// All types of NPCDecorator
class Melee : public NPCDecorator {
public:
	Melee(AbstractNPC *n);
	void modifyHealth(double newHp);
	double getRange();
	double getHealth();
	~Melee();
};

class Ranged : public NPCDecorator {
public:
	Ranged(AbstractNPC *n);
	void modifyHealth(double newHp);
	double getRange();
	double getHealth();
	~Ranged();
};

class Light : public NPCDecorator {
public:
	Light(AbstractNPC *n);
	void modifyHealth(double newHp);
	double getRange();
	double getHealth();
	~Light();
};

class Medium : public NPCDecorator {
public:
	Medium(AbstractNPC *n);
	void modifyHealth(double newHp);
	double getRange();
	double getHealth();
	~Medium();
};

class Heavy : public NPCDecorator {
public:
	Heavy(AbstractNPC *n);
	void modifyHealth(double newHp);
	double getRange();
	double getHealth();
	~Heavy();
};
#endif 